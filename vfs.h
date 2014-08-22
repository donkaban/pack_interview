#ifndef __VFS__
#define __VFS__

#include "common.h"

class vfs
{
public:
    using data_type = std::vector<char> &;
    using data_callback = std::function<data_type (const data_type &)>;


    vfs();
    virtual ~vfs();

    vfs & create_archive(strref);
    vfs & save_archive();
    vfs & unpack_archive(strref, strref postfix = "");
    
    vfs & add_file(strref);
    vfs & add_dir(strref);
    vfs & log_content();

    void onPack(const data_callback &);
    void onUnPack(const data_callback &);

private:
    struct _hdr
    {
        uint8_t  magic       = 42;
        size_t   toc_size    = 0;
        size_t   num_files   = 0;
        size_t   base_offset = 0;
    } header;

    enum class  fileType {FILE,VFS};    
    struct fileinfo
    {
        std::string name;
        size_t      size;
        size_t    offset;
        fileType    type;

        fileinfo(strref name, size_t size, size_t offset, fileType type = fileType::FILE) :
            name(name),
            size(size),
            offset(offset),
            type(type)
        {}
        
        size_t getInfoSize() {return name.size() + 2*sizeof(size_t) + sizeof(fileType);}
    };

    std::vector<fileinfo> tableOfContents;

    size_t current_offset = 0;

    std::ifstream  infile;    
    std::ofstream  outfile;

    data_callback pack_callback     {};
    data_callback unpack_callback   {};

    bool is_dir(strref);

};











#endif




