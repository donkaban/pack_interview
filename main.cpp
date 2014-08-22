#include "common.h"
#include "vfs.h"


void help_usage()
{
	std::cout << "usage: packer packfile infile [infile...] for pack" << std::endl;
	std::cout << "usage: packer packfile for unpack" << std::endl;
	exit(-1);
}

int main(int argc, char **argv)
{
	vfs v;
	if(argc < 2) 
		help_usage();
	try
	{
		if(argc == 2)								// unpack archive
			v.unpack_archive(argv[1], ".unpacked");
		else										// pack archive	
		{	
			v.create_archive(argv[1]);
			for(auto i = 2; i < argc; i++)
				v.add_file(argv[i]);
			v.log_content().save_archive();
		}
	}	

	catch(const std::exception &e)
	{
		std::cout << "error! " << e.what() << std::endl;
	}


	return 0;
}