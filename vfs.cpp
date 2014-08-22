#include "vfs.h"
#include <sys/stat.h>

vfs::vfs()
{}

vfs::~vfs()
{}

vfs & vfs::create_archive(strref name) 
{ 
    outfile.open(name, std::ios::out | std::ios::binary);
    if(!outfile)  
        throw std::runtime_error("can't create file " + name);
    return *this;
}

vfs & vfs::save_archive()
{
    std::cout << "save archive" << std::endl;
    header.num_files = tableOfContents.size();
    outfile.write(reinterpret_cast<const char *>(&header),sizeof(header));
    for(auto &t : tableOfContents)
    {
        size_t name_size = t.name.size();
        outfile.write(reinterpret_cast<const char *>(&name_size),sizeof(size_t));
        outfile.write(reinterpret_cast<const char *>(t.name.c_str()),name_size);
        outfile.write(reinterpret_cast<const char *>(&t.size),sizeof(t.size));
        outfile.write(reinterpret_cast<const char *>(&t.offset),sizeof(t.offset));
        outfile.write(reinterpret_cast<const char *>(&t.type),sizeof(t.type));
        
    }
    for(auto &t : tableOfContents)
    {
        std::vector<char> buff;
        buff.resize(t.size);
        std::cout << "pack file " << t.name << std::endl;
        infile.open(t.name);
        infile.read(&buff[0],t.size);
        if(pack_callback) 
            buff = pack_callback(buff);
        outfile.write(reinterpret_cast<const char *>(&buff[0]),t.size);
        infile.close();
    }
    outfile.flush();
    outfile.close();
    return * this;
} 

vfs & vfs::unpack_archive(strref name, strref postfix)
{
    tableOfContents.clear();
    infile.open(name);
    if(!outfile)  
        throw std::runtime_error("can't open file " + name);
    infile.read(reinterpret_cast<char *>(&header), sizeof(header));
    if(header.magic != 42)
        throw std::runtime_error("it's not a archive " + name);
    std::cout <<  "open pack. toc_size: " << header.toc_size << "  num_files: " << header.num_files << std::endl;
    for(auto i = 0u; i < header.num_files; i++)
    {
        size_t   size;
        char     name[4096];
        size_t   filesize;
        size_t   offset;
        fileType type;

        infile.read(reinterpret_cast<char *>(&size),sizeof(size_t));
        infile.read(reinterpret_cast<char *>(&name),size);
        infile.read(reinterpret_cast<char *>(&filesize),sizeof(filesize));
        infile.read(reinterpret_cast<char *>(&offset),sizeof(offset));
        infile.read(reinterpret_cast<char *>(&type),sizeof(type));
        name[size] = '\0';
        tableOfContents.push_back(fileinfo(name,filesize, offset, type));   
    }   

    for(auto &t : tableOfContents)
    {
        std::vector<char> buff;
        buff.resize(t.size);
        std::cout << "unpack file " << t.name << std::endl;
        outfile.open(t.name+postfix, std::ios::out | std::ios::binary);
        infile.read(&buff[0],t.size);
        if(unpack_callback) 
            buff = unpack_callback(buff);
        outfile.write(reinterpret_cast<const char *>(&buff[0]),t.size);
        outfile.close();
    }
    return *this;
}

vfs & vfs::add_file(strref name) 
{
    infile.open(name);
    if(!infile)
          throw std::runtime_error("can't open file " + name);
    infile.seekg(0, std::ios::end);
    auto filesize = infile.tellg();
    infile.seekg(0, std::ios::beg);
    auto fi = fileinfo(name, filesize, current_offset, is_dir(name) ? fileType::VFS : fileType::FILE);
    tableOfContents.push_back(fi);
    header.toc_size += fi.getInfoSize(); 
    current_offset +=filesize;
    infile.close();
    return * this;
}

vfs & vfs::log_content()    
{
    std::cout << "table of contents size: " << header.toc_size << std::endl;
    std::cout << "num files: " << tableOfContents.size() << std::endl;

    for(auto &t : tableOfContents)
        std::cout << "  + "<< t.name << " size: " << t.size << " offset: " << t.offset << std::endl;
    return * this;
}

void vfs::onPack(const data_callback &cb)    {pack_callback = cb;}
void vfs::onUnPack(const data_callback &cb)  {unpack_callback = cb;}

bool vfs::is_dir(strref name) 
{
  struct stat buffer;   
  return (stat(name.c_str(), &buffer) == 0) && S_ISDIR(buffer.st_mode) ; 
}