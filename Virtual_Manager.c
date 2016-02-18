#include <stdio.h>
#include <string.h>
#define TLB_SIZE 16
#define PAGE_TABLE_SIZE 256
#define NO_OF_FRAMES 256
#define FRAME_SIZE 256

/*
	Defines the stucture of the TLB.
	It contains Page number , Frame number and the valid bit
	If valid bit is zero then The TLB entry is invalid
	If valid bit is one then the TLB entry is valid.
*/
typedef struct T_L_B
{
	int page_number; 		// Represents the Page number		
	int frame_number;		// Represents the Frame number
	int valid_bit;			// Represents the Valid Bit
}T_L_B;

/*
	Represents the Stucture of the Page Table Entry
	It contains frame number , valid bit , count
*/
typedef struct Page_Table_Entry
{
	int frame_number;		// Represents the Frame number
	int valid_bit;			// Represents the valid bit
	int count;				// Represents the time since it has been accessed
} Page_Table_Entry;

/* Define TLB_SIZE TLB entry as TLB*/
T_L_B TLB[TLB_SIZE];

/* Define Page Table to be PAGE_TABLE_SIZE with each element as Page Table Entry */
Page_Table_Entry Page_Table[PAGE_TABLE_SIZE];

/* Define Physical Memory of NO_OF_FRAMES with each frames as FRAME_SIZE */
char Physical_Memory[NO_OF_FRAMES][FRAME_SIZE];

/* Represents the Free Frame */
int Free_Frame[NO_OF_FRAMES];

/* Represents the TLB_HEAD where the next TLB entry should go*/
int TLB_head = 0;

/* 
	This function initializes the TLB and the Page Table 
	Set the valid bit of everything to 0
	and set the initial count to 0
	And every frame is the Free Frame 

*/
void initialization()
{
	int i;
	for( i = 0 ; i < TLB_SIZE ; i++ )
	{
		TLB[i].valid_bit = 0;
	}
	for( i = 0 ; i < PAGE_TABLE_SIZE ; i++ )
	{
		Page_Table[i].valid_bit = 0;
		Page_Table[i].count = 0;
	}
	for( i = 0 ; i < NO_OF_FRAMES ; i++ )
	{
		Free_Frame[i] = 1;
	}
}

/*
	This function is used to Print the TLB
*/
void print_TLB( FILE *fp , int count )
{
	int i;
	for( i = 0 ; i < TLB_SIZE ; i++ )
	{
		fprintf( fp , "\n %d : %d : %d : %d ", i , TLB[i].page_number, TLB[i].frame_number , TLB[i].valid_bit );
	}
}

/*
	This function is used to print the Page Table
*/
void print_Page_Table( FILE *fp , int count )
{
	int i;
	for( i = 0 ; i < PAGE_TABLE_SIZE ; i++ )
	{
		if ( Page_Table[i].valid_bit == 1 )
		fprintf(fp , "\n %d : %d : %d  : %d" , i , Page_Table[i].frame_number , Page_Table[i].count , Page_Table[i].valid_bit );
	}
}

/*
	This function searches the TLB and if found returns the value of the 
	corresponding frame number and if the PAge number is not found in the TLB
	then return -1.
*/
int TLB_Search( int page_number )
{
	int i = 0;
	for( i = 0 ; i < TLB_SIZE ; i++ )
	{
		if( TLB[i].valid_bit == 1 && TLB[i].page_number == page_number )
		{
			return TLB[i].frame_number;
		}
	}
	return -1;
}
/*
	This function searches the TLB and if found returns the value of the 
	corresponding TLB index and if the PAge number is not found in the TLB
	then return -1.
*/
int TLB_index_Search( int page_number )
{
	int i = 0;
	for( i = 0 ; i < TLB_SIZE ; i++ )
	{
		if( TLB[i].valid_bit == 1 && TLB[i].page_number == page_number )
		{
			return i;
		}
	}
	return -1;
}
/*
	This function is used to search the value of the given page number
	in the Page table if the entry is valid then return the frame number
	and if it is invalid then return the value of -1.
*/
int Page_Table_Search( int page_number )
{
	if( Page_Table[page_number].valid_bit == 1 )
	{
		return Page_Table[page_number].frame_number;
	}
	else
	{
		return -1;
	}
	return -1;
}

/*
	This function is used to search the free slot in the physical memory
	If any free frame is found then return the index of the free frame
	or else return -1.
*/
int search_for_free_slot_in_physical_memory()
{
	int i;
	for( i = 0 ; i < NO_OF_FRAMES ; i++ )
	{
		if( Free_Frame[i] == 1 )
		{
			return i;
		}
	}
	return -1;
}
/*
	This function is used to select the victim page from the available pages.
	We implemented LRU Algorithm Using Counters. So Which Page has the largest
	Count is said to be used Last So it is used as the Victim page and its frame 
	is used as the new frame for the page faulted page.
*/
int select_victim_page()
{
	int i;
	int max = 0;
	int frame = -1;
	int page = -1;
	for( i = 0 ; i < PAGE_TABLE_SIZE ; i++ )
	{
		if( Page_Table[i].valid_bit == 1 )				// For all Page Table Entry which is  valid
		{
			if( Page_Table[i].count >= max )			// Find the maximum value of the count
			{
				max = Page_Table[i].count;
				frame =  Page_Table[i].frame_number;
				page = i;								// Assign that index to the victim page number 
			}
		}
	}
	return page;										// Return the victim page index
}

/*
	This function is used to Update the count of each pages by one which are valid
	and set the page being used count to zero since we are implementing LRU Algorithm
*/
int Update_Count( int page_number )
{
	int i;
	for( i = 0 ; i < PAGE_TABLE_SIZE ; i++ )
	{
		if( Page_Table[i].valid_bit == 1 )
		{
			Page_Table[i].count++;				// increase the valid Page table entries count by 1
		}
	}
	Page_Table[page_number].count = 0;			// set the page being used count to zero
	return 0;
}
/*
	This function has been used to Update the TLB Entries
*/
int Update_TLB( int page_number , int frame_number )
{
	int i;
	/*
		Search the TLB for invalid entries if any found replace that entry
		If nothing found then ... 
	*/
	for( i = 0 ; i < TLB_SIZE ; i++ )
	{
		if( TLB[i].valid_bit == 0 )
		{
			TLB[i].page_number = page_number;
			TLB[i].frame_number = frame_number;
			TLB[i].valid_bit = 1;
			return 0;
		}
	}
	/*
		Implement FIFO Algorithm using the Circular Queue.
	*/
	TLB[TLB_head % 16].valid_bit = 1;
	TLB[TLB_head % 16].page_number = page_number;
	TLB[TLB_head % 16].frame_number = frame_number;
	TLB_head++;
	return 0;
}
/*
	This function is used to update the corresponding page Table Entry
*/
int Update_Page_Table( int page_number , int frame_number )
{
	Page_Table[page_number].valid_bit = 1;					// Make the valid bit to one
	Page_Table[page_number].frame_number = frame_number;	// Assign the corresponding frame number
}

/*
	This function is used to Update the Free Frame array which represents
	the Free Slots in the physical Memory
*/
int Update_Free_Frame( int frame_number )
{
	Free_Frame[frame_number] = 0;
}

/*
	Given Frame number and offset this function reads from the Physical Memory
	and writes them and the Physical Address and the value stored at that address
	in the output_file
*/
int Read_from_Memory( int number , int frame_number , int offset , FILE *output_file )
{
	int value = Physical_Memory[frame_number][offset];
	fprintf( output_file , " The virtual Address : %d , Physical_Address : %d , value : %d \n", number , frame_number*256 + offset , value ); 
	return 0;					
}

/*
	This function used to copy the elements in the BACKING_STORE.bin stored
	at given page number into the physical memory at the corresponding
	given frame number. 
*/
int Copy_from_disk_to_memory( int frame_number , int page_number )
{
	int i;
	FILE *fp;
	char buffer;
	fp = fopen("BACKING_STORE.bin","rb");
	fseek( fp , page_number*256 , SEEK_SET );		// Go to the corresponding Page
	for( i = 0 ; i < FRAME_SIZE ; i++ )
	{
		fread( &buffer , 1 , 1 , fp );				// Used to read 1 byte and store in buffer
		Physical_Memory[frame_number][i] = buffer;	// Store buffer value in physical memory
	}
	fclose(fp);
	Update_Free_Frame( frame_number );
}
int main()
{
	char ADDRESS[10];									// Used to get address from file in string
	int number_of_TLB_miss = 0;							// Number of TLB Miss
	int number_of_Page_fault = 0;						// Number of Page Faults
	int number;											// Used to represent ADDRESS in integer
	int x;												// Last 16 bits of the number
	int page_number;									// First 8 bits of the x
	int offset;											// LAst 8 bits of the x
	int TLB_Output = -1;								// Represents the Outcome of TLB search
	int page_table_output = -1;							// Output of Page Table Search
	int free_frame_output = -1;							// Output of Free Frame search
	FILE *read_file;					
	read_file = fopen("addresses.txt", "r");			// Reading from File Addresses.txt
	FILE *output_file;
	output_file = fopen("output.txt","w");				// Writing Output in the File output.txt
	FILE *steps;
	steps = fopen("steps.txt","w" );
	int count = 0;
	initialization();									// Initialize Everything
	
	
	/*
		read The File Until we Reach the End of the File
	*/
	while( fgets( ADDRESS , sizeof ADDRESS , read_file ) != NULL )
	{
		count++;
		/* Convert the string we read to integer */
		number = atoi(ADDRESS);
		/* Masking the last 16 bits of the number */
		x =  ( number & 0x0000FFFF );
		/* Masking the first 8 bits of the number x: 16 bits and shifting by 8 to get page number*/
		page_number = ( x & 0x0000FF00 ) >> 8;
		/*The last 8 bits represents the offset */
		offset = ( x & 0x000000FF );
		
		fprintf(steps,"\n Count is %d \n", count );	
		fprintf(steps,"\n\n The page number is %d \n", page_number );
		fprintf(steps,"\n The offset is %d \n", offset );
		fprintf(steps,"\n TLB \n ");
		print_TLB(steps,count); 
		fprintf(steps,"\n Page Table \n ");
		print_Page_Table(steps,count);
		
		/* Search in the TLB for the given Page number If is -1 then It is a TLB miss */
		TLB_Output = TLB_Search( page_number );

		if( TLB_Output == -1 )
		{
			
			fprintf(steps,"\n TLB Miss \n");
			/* Increase the number of TLB miss by 1 */
			number_of_TLB_miss++;
			
			/* Search the Page number in the Page Table */
			page_table_output = Page_Table_Search(page_number);
			
			/* If return value is -1 then the page entry is not valid so it is Page Fault*/
			if( page_table_output == -1 )
			{
				
				fprintf(steps,"\n Page Fault \n");
				/* Increase the number of Page Fault by 1 */
				number_of_Page_fault++;
				
				/* Check for a free frame in the physical Memory to bring in the page */
				free_frame_output = search_for_free_slot_in_physical_memory();
				
				/* If there are no free in the physical Memory We have to select a victim page */
				if( free_frame_output == -1 )
				{

					int victim_page = select_victim_page();
					fprintf(steps,"\n The Selected Victim Page is %d ", victim_page );
					/* Make the Page TAble Entry for Selected Victim page to be invalid */
					Page_Table[victim_page].valid_bit = 0;
					/* Search For the Entry in TLB and make it invalid */
					int in_TLB = TLB_index_Search( victim_page );
					if( in_TLB != -1 )
					{
						TLB[in_TLB].valid_bit = 0;
					}
					/* Get the victim page frame number */
					int victim_frame = Page_Table[victim_page].frame_number;
					/* Copy the page from the BACKING_STORE.bin into the victim frame */
					Copy_from_disk_to_memory( victim_frame , page_number );
					/* Update the TLB */
					Update_TLB( page_number , victim_frame );
					/* Update the Page table Entries*/
					Update_Page_Table( page_number , victim_frame );
					/* Update the Count of all Unsed Valid Page Table Entry */
					Update_Count(page_number);
					/* Then Read from the physical memory from the victim_frame */
					Read_from_Memory( number , victim_frame , offset , output_file );
				}
				else
				{
					/* Copy the page into the free frame after the page fault occouues*/
					Copy_from_disk_to_memory( free_frame_output , page_number );
					/* Update TLB */
					Update_TLB( page_number , free_frame_output );
					/* Update Page Table */
					Update_Page_Table( page_number , free_frame_output );
					/* Update Count */
					Update_Count(page_number);
					/* Read from the memory after translation from the physical memory */
					Read_from_Memory( number , free_frame_output , offset , output_file );
				}
			}
			else
			{
					/* Update the TLB after found in the Page Table */
					Update_TLB( page_number , page_table_output );
					/* Update the Count */
					Update_Count(page_number);
					/* Read from the Physical Memory */
					Read_from_Memory( number , page_table_output , offset , output_file );
			}
		}
		else
		{
			/* Update the Count */
			Update_Count(page_number);
			/* Read from the Physial Memory */
			Read_from_Memory( number , TLB_Output , offset , output_file );	
		}
	}
	/* Close the Files */
	fclose(read_file);
	fclose(output_file);
	printf("\n The number of Page Faults are : %d ",number_of_Page_fault );
	printf("\n The number of TLB Miss are : %d ",number_of_TLB_miss );
	return 0;
}

