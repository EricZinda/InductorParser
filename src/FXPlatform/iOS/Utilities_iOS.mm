#include "Utilities.h"
#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>
#import <CloudKit/CloudKit.h>
using namespace std;
 
uint16_t GetBuild()
{
    NSBundle* main = [NSBundle mainBundle];
    NSDictionary<NSString *,id> *infoDictionary = [main infoDictionary];
    return lexical_cast<uint16_t>([[infoDictionary objectForKey:@"CFBundleVersion"] UTF8String]);
}


