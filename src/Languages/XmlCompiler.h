//
//  XmlCompiler.h
//  GameLib
//
//  Created by Eric Zinda on 11/8/15.
//  Copyright (c) 2015 Eric Zinda. All rights reserved.
//

#ifndef __GameLib__XmlCompiler__
#define __GameLib__XmlCompiler__

#include "FXPlatform/Parser/Compiler.h"
#include "Languages/HtmlParser.h"
#include <map>
using namespace std;
using namespace FXPlat;
using namespace Html;

/*
	The XML Compiler is using the same parser as the HTML Compiler as an approximation
	since the HTML compiler is strict about start and end tags, etc.

	It is using a very simple form of XML, doesn't recognize many of the advanced features, etc.
	Should be able to parse the simple examples from here: https://www.w3schools.com/xml/xml_examples.asp
*/

class XmlNode
{
public:
    vector<shared_ptr<XmlNode>> &childNodes() { return m_children; }
    void AppendChild(shared_ptr<XmlNode> node) { m_children.push_back(node); }
    
    typedef vector<shared_ptr<XmlNode>> XmlNodeList;
    virtual string ToString() { return nodeName() + "(" + nodeValue() +") {" + ToStringNodes() + "} "; }
    
protected:
    string ToStringNodes()
    {
        stringstream stream;
        for(XmlNodeList::iterator iter = m_children.begin(); iter != m_children.end(); ++iter)
        {
            stream << (*iter)->ToString();
        }
        
        return stream.str();
    }
    
    ValueProperty(protected, string, nodeName);
    ValueProperty(protected, string, nodeValue);
    XmlNodeList m_children;
};

class XmlElement : public XmlNode
{
public:
    typedef map<string, string> AttributesType;
    
    AttributesType &attributes() { return m_attributes; }
    void SetAttribute(const string &name, const string &value)
    {
        m_attributes[name] = value;
    }
    
    string GetAttribute(const string &name)
    {
        return m_attributes[name];
    }
    
    bool HasAttribute(const string &name)
    {
        return m_attributes.find(name) != m_attributes.end();
    }
    
    virtual string ToString() { return "<" + nodeName() + " " + ToStringAttributes() + ">" + ToStringNodes() + "</" + nodeName() + ">"; }
    
private:
    string ToStringAttributes()
    {
        stringstream stream;
        for(AttributesType::iterator iter = m_attributes.begin(); iter != m_attributes.end(); ++iter)
        {
            stream << (*iter).first << "='" + (*iter).second + "' ";
        }
        
        return stream.str();
    }
    AttributesType m_attributes;
};

class XmlCompiler : public Compiler<HtmlDocumentRule>
{
public:
    virtual bool ProcessAst(shared_ptr<CompileResultType> ast)
    {
        result(ParseNode((*ast)[0]));
        return errors().size() == 0;
    }
    
    // Can return nullptr if there is an error
    shared_ptr<XmlNode> ParseNode(shared_ptr<Symbol> elementSymbol)
    {
        switch(elementSymbol->symbolID())
        {
            case HtmlSymbolID::voidElement:
                return ParseStartTag(elementSymbol);
                break;
                
            case HtmlSymbolID::element:
            {
                // Create the element
                shared_ptr<XmlElement> element = ParseStartTag(elementSymbol);
                
                // recurse on children
                int elementSymbolIndex = 1;
                shared_ptr<Symbol> elementChildSymbol = GetChild(elementSymbol, elementSymbolIndex++, -1);
                while(elementChildSymbol != nullptr)
                {
                    if(elementChildSymbol->symbolID() == HtmlSymbolID::endTag)
                    {
                        break;
                    }
                    
                    shared_ptr<XmlNode> elementChild = ParseNode(elementChildSymbol);
                    if(elementChild != nullptr)
                    {
                        element->AppendChild(elementChild);
                        elementChildSymbol = GetChild(elementSymbol, elementSymbolIndex++, -1);
                    }
                    else
                    {
                        // Error parsing child
                        return nullptr;
                    }
                }
                
                // Confirm end tag matches start
                string startTagName = element->nodeName();
                string endTagName = GetChild(elementChildSymbol, 0, HtmlSymbolID::tagName)->ToString();
                if(startTagName != endTagName)
                {
                    errors().push_back(CompileError(-1, -1, "Start tag '" + startTagName + "' does not match end tag '" + endTagName +"'"));
                    return nullptr;
                }
                
                // return it
                return element;
            }
                break;
                
            case HtmlSymbolID::text:
            {
                shared_ptr<XmlNode> textNode = shared_ptr<XmlNode>(new XmlNode());
                textNode->nodeName("#text");
                textNode->nodeValue(elementSymbol->ToString());
                return textNode;
            }
                
            default:
                errors().push_back(CompileError(-1, -1, "Unknown node '" + elementSymbol->ToString() + "'"));
                return nullptr;
        }
    }
    
    shared_ptr<XmlElement> ParseStartTag(shared_ptr<Symbol> elementSymbol)
    {
        int startTagSymbolIndex = 0;
        shared_ptr<Symbol> startTagSymbol = GetChild(elementSymbol, 0, HtmlSymbolID::startTag);
        string startTagName = GetChild(startTagSymbol, startTagSymbolIndex++, HtmlSymbolID::tagName)->ToString();
        
        shared_ptr<XmlElement> element = shared_ptr<XmlElement>(new XmlElement());
        element->nodeName(startTagName);
        
        // Add the attributes
        shared_ptr<Symbol> attributesSymbol = GetChild(startTagSymbol, startTagSymbolIndex++, HtmlSymbolID::attributes);
        
        int attributesSymbolIndex = 0;
        shared_ptr<Symbol> attributeSymbol = GetChild(attributesSymbol, attributesSymbolIndex++, HtmlSymbolID::attribute);
        while(attributeSymbol != nullptr)
        {
            shared_ptr<Symbol> attributeValueSymbol = GetChild(attributeSymbol, 1, HtmlSymbolID::attributeValue);
            element->SetAttribute(
                                  GetChild(attributeSymbol, 0, HtmlSymbolID::attributeName)->ToString(),
                                  attributeValueSymbol == nullptr ? "" : attributeValueSymbol->ToString());
            attributeSymbol = GetChild(attributesSymbol, attributesSymbolIndex++, HtmlSymbolID::attribute);
        }
        
        return element;
    }
    
protected:
    ValueProperty(private, shared_ptr<XmlNode>, result);
};

#endif /* defined(__GameLib__XmlCompiler__) */
