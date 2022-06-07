// On-disk file system format.
// Both the kernel and user programs use this header file.

#define ROOTINO 1   // root i-number
#define BSIZE 1024  // block size

// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]
//
// mkfs computes the super block and builds an initial file system. The
// super block describes the disk layout:
struct superblock {
  uint magic;       // Must be FSMAGIC
  uint size;        // Size of file system image (blocks)
  uint nblocks;     // Number of data blocks
  uint ninodes;     // Number of inodes.
  uint nlog;        // Number of log blocks
  uint logstart;    // Block number of first log block
  uint inodestart;  // Block number of first inode block
  uint bmapstart;   // Block number of first free map block
};

#define FSMAGIC 0x10203040

#define NDIRECT 10
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)

// On-disk inode structure
struct dinode {
  short type;               // File type
  short major;              // Major device number (T_DEVICE only)
  short minor;              // Minor device number (T_DEVICE only)
  short nlink;              // Number of links to inode in file system
  uint size;                // Size of file (bytes)
  uint addrs[NDIRECT + 1];  // Data block addresses
  short uid;                // Owner uid
  short gid;                // Group id
  short mode;                // File mode
};

// Inodes per block.
#define IPB (BSIZE / sizeof(struct dinode))

// Block containing inode i
#define IBLOCK(i, sb) ((i) / IPB + sb.inodestart)

// Bitmap bits per block
#define BPB (BSIZE * 8)

// Block of free map containing bit for block b
#define BBLOCK(b, sb) ((b) / BPB + sb.bmapstart)

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

// Permission
#define O_READ 0x01
#define O_WRITE 0x02
#define O_EXEC 0x04

#define P_RUSR 0b100000000
#define P_WUSR 0b010000000
#define P_XUSR 0b001000000

#define P_RGRP 0b000100000
#define P_WGRP 0b000010000
#define P_XGRP 0b000001000

#define P_ROTH 0b000000100
#define P_WOTH 0b000000010
#define P_XOTH 0b000000001

struct dirent {
  ushort inum;
  char name[DIRSIZ];
};
