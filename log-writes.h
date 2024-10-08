#ifndef _LOG_WRITES_H_
#define _LOG_WRITES_H_

#include <fcntl.h>
#include <linux/byteorder/little_endian.h>
#include <linux/types.h>

extern int log_writes_verbose;

#define le64_to_cpu __le64_to_cpu
#define le32_to_cpu __le32_to_cpu

typedef __u64 u64;
typedef __u32 u32;

#define LOG_FLUSH_FLAG (1 << 0)
#define LOG_FUA_FLAG (1 << 1)
#define LOG_DISCARD_FLAG (1 << 2)
#define LOG_MARK_FLAG (1 << 3)
#define LOG_METADATA_FLAG (1 << 4)

#define WRITE_LOG_VERSION 1
#define WRITE_LOG_MAGIC 0x6a736677736872

/*
 * Basic info about the log for userspace.
 */
struct log_write_super {
  __le64 magic;
  __le64 version;
  __le64 nr_entries;
  __le32 sectorsize;
};

/*
 * sector - the sector we wrote.
 * nr_sectors - the number of sectors we wrote.
 * flags - flags for this log entry.
 * data_len - the size of the data in this log entry, this is for private log
 * entry stuff, the MARK data provided by userspace for example.
 */
struct log_write_entry {
  __le64 sector;
  __le64 nr_sectors;
  __le64 flags;
  __le64 data_len;
};

#define LOG_IGNORE_DISCARD (1 << 0)
#define LOG_DISCARD_NOT_SUPP (1 << 1)

struct log {
  int logfd;
  int replayfd;
  unsigned long flags;
  u64 sectorsize;
  u64 nr_entries;
  u64 cur_entry;
  u64 max_zero_size;
  off_t cur_pos;
};

struct log *log_open(char *logfile, char *replayfile);
int log_replay_next_entry(struct log *log, struct log_write_entry *entry,
                          int read_data, int dump_only);
int log_seek_entry(struct log *log, u64 entry_num);
int log_seek_next_entry(struct log *log, struct log_write_entry *entry,
                        int read_data);
void log_free(struct log *log);

#endif
