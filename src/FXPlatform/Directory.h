#pragma once
#include <memory>
#include <string>
#include <vector>
#include "FileStream.h"
using namespace std;

#define PATH_SEPARATOR "/"
#define PATH_SEPARATOR_CHAR '/'

class Directory : public std::enable_shared_from_this<Directory>
{
public:
    Directory(const std::string &path, bool ensure = true);
    
    // https://developer.apple.com/library/ios/documentation/FileManagement/Conceptual/FileSystemProgrammingGuide/FileSystemOverview/FileSystemOverview.html
    // Where anything not created by the user should be placed
    static std::shared_ptr<Directory> ApplicationFolder();
    
    // Where the app itself is stored
    static std::shared_ptr<Directory> InstallationFolder();
    
    // Where saved games or anything that should be backed up should be saved
    static std::shared_ptr<Directory> DocumentsFolder();
    
    static string AddPathIfNoPath(const string &file, Directory &defaultPath);
	void Clear();
    static string Combine(const string &path1, const string &path2);
    static bool Compare(vector<uint8_t> source, const string &targetPathAndFile);
    // Silently replaces the Destination file if it exists
    static void CopyFile(const string &pathAndFileSrc, const string &pathAndFileDst);
    static void CopyFolder(const string &pathSrc, const string &pathDst);
    void CreateLocalFolder(const string &name);
    static void CreateFolderPath(const string &name);
    static void DeleteItem(const string &path);
	void DeleteLocalDirectory(const string &name);
    void DeleteLocalFile(const string &name);
    static bool FileExists(const string &fullPath);
    static bool FolderExists(const string &fullPath);
    string FullPathFromLocalPath(const string &localPath);
    static string GetDirectory(const string &pathAndFile);
    string GetDirectoryPath();
    shared_ptr<vector<string> > GetFileNames(const string &pattern, bool includeDirectories = false, bool includeFiles  = true);
    shared_ptr<vector<string> > GetFullPaths(const string &pattern, bool includeDirectories = false, bool includeFiles  = true);
    static string GetFilenameWithoutExtension(const string &pathAndFile);
    shared_ptr<Directory> GetFolder(const string &folderName);
    string GetUniqueFilename(const string &nameNoExtension, const string &extension);
    static bool HasNoPath(const string &file);
    static bool ItemExists(const string &fullPath);
    bool LocalFileExists(const string &name);
    bool LocalFolderExists(const string &name);
    string MakeRelative(const string &fullPath);
    shared_ptr<FileStream> OpenFile(const string &filename, FileOpenStyle openStyle, AccessRights rights);
    // Sets the default folders back to their system defaults
    static void ResetFoldersToSystemDefaults();
    // Supports temporarily revectoring where data gets stored and loaded from so that tests can be isolated from each other and the harness
    static void SetTestRoot(const string testRootFolder);
    // Supports calling Directory from apps that don't have winrt permission by avoiding calls to ApplicationFolder()
    static void SetTestRootApplicationFolder(const string testRootFolder);
    static void SplitPath(const string &pathAndFile, string &path, string &fileWithoutExtension, string &extension);

private:
    Directory();
    void SetFolder(const string &path);

    string m_folder;
    static string m_rootApplicationFolder;
    static string m_rootDocumentsFolder;
    static string m_rootInstallationFolder;
};

