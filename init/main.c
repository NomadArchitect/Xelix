#include <common/multiboot.h>
#include <common/generic.h>
#include <devices/display/interface.h>
#include <devices/cpu/interface.h>
#include <devices/keyboard/interface.h>
#include <memory/interface.h>
#include <interrupts/interface.h>
#include <devices/pit/interface.h>
#include <memory/kmalloc.h>
#include <filesystems/interface.h>
#include <filesystems/memfs/interface.h>

void checkIntLenghts();


void kmain(multibootHeader_t *mboot_ptr)
{

	
	
	memory_init_preprotected();
	interrupts_init();
	
	
	
	// check that our initrd was loaded by the bootloader and determine the addresses.
	ASSERT(mboot_ptr->mods_count > 0);
	uint32 initrd_location = *((uint32*)mboot_ptr->mods_addr);
	uint32 initrd_end = *(uint32*)(mboot_ptr->mods_addr+4);
	// Don't trample our module with placement accesses, please
	kmalloc_init(initrd_end);
	
	
	display_init();
	
	
	log_init();
	
	
	display_setColor(0x0f);
	print("\n");
	print("                                   Xelix\n");
	print("\n");
	display_setColor(0x07);
	
	
	print("a");
	

	// needed before display_init!!!
	log("Initialized preprotected memory\n");
	log("Initialized Display.\n");
	
	
	cpu_init();
	log("Initialized CPU\n");
	
	log("Initialized interrupts\n");
	memory_init_postprotected();
	log("Initialized postprotected memory\n");
	pit_init(50); //50Hz
	log("Initialized PIT (programmable interrupt timer)\n");
	keyboard_init();
	log("Initialized keyboard\n");
	
	display_setColor(0x0f);
	log("Xelix is up.\n");
	display_setColor(0x07);
	log("Reading Initrd...\n");
	//setLogLevel(0);
	log("Listing files of initrd");
	// Initialise the initial ramdisk, and set it as the filesystem root.
	fsRoot = memfs_init(initrd_location);
	
	/*
	// list the contents of /
	int i = 0;
	struct dirent *node = 0;
	while ( (node = readdirFs(fsRoot, i)) != 0)
	{
		//if(strcmp(node->name, "helloworld.bin") ==0)
		//	continue;
		print("Found file ");
		print(node->name);
		fsNode_t *fsnode = finddirFs(fsRoot, node->name);
		if ((fsnode->flags&0x7) == FS_DIRECTORY)
			print("\n	 (directory)\n");
		else
		{
			print("\n	  contents: \"");
			char buf[256];
			uint32 sz = readFs(fsnode, 0, 256, buf);
			int j;
			for (j = 0; j < sz; j++)
				if(j < fsnode->length -1)
					display_printChar(buf[j]);
			
			print("\"\n");
		}
		i++;
	}
	*/

	print("finished listing files\n");


	// trigger page fault!
	
	uint32* a;
	a = 1024*1024*1024; // 4gb
	*a = 1234;
	printDec(*a);
	
	
	printHex(sizeof(size_t));
	
	
	print("HALLO");
	
	print("a");
	
	
	while(1)

	{
		
	}
}


void checkIntLenghts()
{
	log("Checking length of uint8... ");
	ASSERT(sizeof(uint8) == 1);
	log("Right\n");
	
	log("Checking length of uint16... ");
	ASSERT(sizeof(uint16) == 2);
	log("Right\n");
	
	log("Checking length of uint32... ");
	ASSERT(sizeof(uint32) == 4);
	log("Right\n");
}
