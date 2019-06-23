typedef struct
{
	char id[4]; // Should be 'PACK'
	int dstart; // Offest in the file to the directory
	int dsize;  // Size in bytes of the directory, same as num_items*64
} pak_header_t;

typedef struct
{
	char name[56]; // The name of the item, normal C string
	int start; // Offset in .pak file to start of item
	int size; // Size of item in bytes
} pak_item_t;

// here temporarily? -tkidd

/* the glcmd format:
 * - a positive integer starts a tristrip command, followed by that many
 *   vertex structures.
 * - a negative integer starts a trifan command, followed by -x vertexes
 *   a zero indicates the end of the command list.
 * - a vertex consists of a floating point s, a floating point t,
 *   and an integer vertex index. */

typedef struct
{
    int ident;
    int version;
    
    int skinwidth;
    int skinheight;
    int framesize;  /* byte size of each frame */
    
    int num_skins;
    int num_xyz;
    int num_st;     /* greater than num_xyz for seams */
    int num_tris;
    int num_glcmds; /* dwords in strip/fan command list */
    int num_frames;
    
    int ofs_skins;  /* each skin is a MAX_SKINNAME string */
    int ofs_st;     /* byte offset from start for stverts */
    int ofs_tris;   /* offset for dtriangles */
    int ofs_frames; /* offset for first frame */
    int ofs_glcmds;
    int ofs_end;    /* end of file */
} dmdl_t;

// end tkidd
