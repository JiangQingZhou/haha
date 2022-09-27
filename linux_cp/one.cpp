#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstdio>
#include <string>

#if __has_include(<filesystem>)
# include <filesystem>
#endif // __has_include

using namespace std;
namespace fs = std::filesystem;

// traverse the directory
void dfs_dir(fs::path str, fs::path str2)
{
    // Create a end iterator by default constructor
    fs::directory_iterator end_itr;
    for (fs::directory_iterator i(str), j(str2); i != end_itr && j != end_itr; ++i, ++j)
    {
        fs::directory_entry entry(i->path());
        fs::directory_entry entry2(j->path());
        if (entry.status().type() == fs::file_type::directory)
        {
            dfs_dir(i->path(), j->path());
        }
        if (entry.status().type() != entry2.status().type())
        {
            //printf(i.path().string().c_str());
            printf("error!\n");
        }
	}
}

int main()
{
    fs::path str = "/home/aba/Desktop/linux-5.19.10";
    fs::path str2 = "/home/aba/Desktop/linux-5.19.10bak";
    const char* dir1 = str.c_str();
    const char* dir2 = str2.c_str();

//    try
//    {
//        fs::copy(dir1, dir2,
//            fs::copy_options::update_existing
//            |fs::copy_options::copy_symlinks
//            |fs::copy_options::recursive);
//    }
//    catch (const fs::filesystem_error& e) {
//        printf(e.what());
//    }

    dfs_dir(str, str2);

    exit(0);
}
