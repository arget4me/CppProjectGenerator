#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>


static const std::string TEMPLATE_PATH_ENV("CPP_TEMPLATE_PATH");
static const std::string UNSET("UNSET");
static const std::string CONFIG_FILE("config.bat");

#include <sys/types.h>
#include <sys/stat.h>
bool DirectoryExists(std::string path)
{
    struct stat info;
    if(stat(path.c_str(), &info) != 0)
    {
        // Can't access file
        return false;
    }
    else if(info.st_mode & S_IFDIR)
    {
        // Is a directory
        return true;
    }

    // Not a directory
    return false;
}

static inline void PrintCommandlineArguments(int argc, char* argv[])
{
    for(int i = 0; i < argc; i++)
    {
        std::cout << argv[i] << " ";
    }
    std::cout << "\n";
}

#include <windows.h>
#include <shellapi.h>
void OpenExplorer(std::string path)
{
    ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
}

void RunBatFile(std::string batFile)
{
    ShellExecuteA(NULL, batFile.c_str(), "", NULL, NULL, SW_SHOWDEFAULT);
}

#include <filesystem>
// Recursively copies all files and folders from src to target and overwrites existing files in target.
void CopyRecursive(const std::filesystem::path& src, const std::filesystem::path& target) noexcept
{
    try
    {
        std::filesystem::copy(src, target, std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
    }
    catch (std::exception& e)
    {
        std::cout << e.what();
    }
}

int main(int argc, char* argv[])
{
    PrintCommandlineArguments(argc, argv);

    if (argc < 2)
    {
        std::cout << "ERROR:\n" << "Not enough arguments. Use this format: CPPGen.exe projectName {projectPath}\n";
        return -2;
    }

    std::string projectName(argv[1]);
    std::string projectPath = UNSET;
    if(argc >= 3)
    {
        projectPath = std::string(argv[2]);
    }
    else
    {
        projectPath = std::string(std::filesystem::current_path().u8string() + "\\" + projectName);
    }
    
    if(!DirectoryExists(projectPath))
    {
        std::cout << "Project path doesn't exist. Creating the project path " << projectPath << "\n";
        try{
            // @TODO: Fix code. Not very nice code but gets the job done for now...

            
            std::string createFoderPathStepByStep;

            std::string createFolderPath = projectPath;
            
            {
                // Check if path begins with a drive path, eg. c:/ or E:/
                auto colonIndex = createFolderPath.find_first_of(":");
                if(colonIndex != std::string::npos)
                {
                    createFoderPathStepByStep = createFolderPath.substr(0, colonIndex + 1) + "\\";
                    createFolderPath = createFolderPath.substr(colonIndex + 1);
                    if (createFolderPath.find_first_of("/") != std::string::npos || createFolderPath.find_first_of("\\") != std::string::npos)
                    {
                        createFolderPath = createFolderPath.substr(1);
                    }
                }
            }

            auto createFolderPathSubdir = createFolderPath;
            {
                // Iterate thorugh all sub directories and create them.
                // This doesn't check if they exist. Expects that it will just silently fail if they do
                auto slashIndex = createFolderPathSubdir.find_first_of("/");
                auto slashIndex2 = createFolderPathSubdir.find_first_of("\\");

                while (slashIndex != std::string::npos || slashIndex2 != std::string::npos)
                {
                    auto index = slashIndex;
                    if(slashIndex2 < index)
                    {
                        index = slashIndex2;
                    }

                    createFoderPathStepByStep += createFolderPathSubdir.substr(0, index);
                    createFolderPathSubdir = createFolderPathSubdir.substr(index + 1);
                    slashIndex = createFolderPathSubdir.find_first_of("/");
                    slashIndex2 = createFolderPathSubdir.find_first_of("\\");

                    // std::cout << "Trying to create sub-path: " << createFoderPathStepByStep << " { " << createFolderPathSubdir << " }\n";
                    std::filesystem::create_directory(createFoderPathStepByStep);
                    createFoderPathStepByStep += "\\";
                }
            }

            if (createFolderPathSubdir.empty() == false)
            {
                // Create the final directory
                createFoderPathStepByStep += createFolderPathSubdir;
                std::cout << "Trying to create path: " << createFoderPathStepByStep << "\n";
                if(!std::filesystem::create_directory(createFoderPathStepByStep))
                {
                    std::cerr << "Failed to create a directory\n";
                }
            }
            else
            {
                std::cerr << "A path can't end with a / or \\";
                return -4;
            }

        }catch(const std::exception& e){
            std::cerr << e.what() << '\n';
            return -3;
        }        
    }

    std::cout << "Creating project " << projectName << " in project path " << projectPath << "\n";

    std::string templatePath = UNSET;
    {
        const char* path = std::getenv(TEMPLATE_PATH_ENV.c_str());
        if(path != nullptr)
        {
            templatePath = std::string(path);
        }
    }

    std::cout << "Environment variable " << TEMPLATE_PATH_ENV << " is set to { " << templatePath << " }\n";
    if (templatePath == UNSET)
    {
        std::cout << "\nERROR:\nPlease add the environment variable " << TEMPLATE_PATH_ENV << " with the value set to the path for the template cpp code/folder sturcture.\n";
        return -1;
    }

    if (!DirectoryExists(templatePath))
    {
        std::cout << "\nERROR:\n" << templatePath << " is not a valid directory please update it or create the template folder at that location.\n";
        return -1;
    }

    CopyRecursive(templatePath, projectPath);
    std::filesystem::current_path(std::filesystem::path(projectPath));
    std::ofstream ofs(CONFIG_FILE);
    ofs << "set PROJECT_NAME=" << projectName <<"\n"; 
    ofs.close();

    std::cout << "Successfully created project " << projectName << "\n";
    // OpenExplorer(std::filesystem::current_path().u8string());
    RunBatFile("edit.bat");

}