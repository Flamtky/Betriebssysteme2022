#include "kernel/types.h"
#include "kernel/net.h"
#include "kernel/stat.h"
#include "user/user.h"

#define GT +2

// see https://datatracker.ietf.org/doc/html/rfc1361
char *month[] = {"Jan", "Feb", "Mar", "Apr", "Mai", "Jun",
                 "Jul", "Aug", "Sep", "Oct", "Nov", "Dez"};
int daysOfMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void printDate(uint32 sec) {
  uint32 min = sec / 60;
  uint32 h = min / 60;
  uint32 d = h / 24;
  uint32 y = 1900;
  int i = 0;

  uint32 comp_val = ((y + 1) % 4 != 0) ? 365 : 366;
  while (d >= comp_val) {
    d -= comp_val;
    y++;
    comp_val = ((y + 1) % 4 != 0) ? 365 : 366;
  }
  d++;  // d=0 -> 1 Jan
  while (d > daysOfMonth[i] || (i == 1 && y % 4 == 0 && d > daysOfMonth[i])) {
    d -= daysOfMonth[i];
    if (i == 1 && y % 4 == 0) d--;
    i++;
  }

  printf("Date: %l %s. %l, %l:%l:%l\n", d, month[i], y, (h % 24) + GT, min % 60,
         sec % 60);
}

void sntp(void) {
  // request ptbtime1.ptb.de (192.53.103.108)

  struct sntp *sntp_req = (struct sntp *)malloc(sizeof(struct sntp));

  // request header
  sntp_req->lvm = 0b11100011; // No warning; current version is 4; client

  sntp_req->stratum = 0; // kiss of death (for reference id)
  sntp_req->poll = 0; 
  sntp_req->precision = 0;

  // request data
  sntp_req->root_delay = 0;
  sntp_req->root_dispersion = 0;
  sntp_req->reference_id = 0;

  sntp_req->reference_timestamp = 0;
  sntp_req->originate_timestamp = 0;
  sntp_req->receive_timestamp = 0;
  sntp_req->transmit_timestamp = 0;

  // request
  int fd;
  uint32 dst = (192 << 24) | (53 << 16) | (103 << 8) | (108 << 0);
  fd = connect(dst, 25565, 123);
  
  // send request
  if (fd < 0) {
    printf("sntp: connect() failed\n");
    exit(1);
  }

  if (write(fd, sntp_req, sizeof(struct sntp)) < 0) {
    printf("sntp: write() failed\n");
    exit(1);
  }

  // receive response
  if (read(fd, sntp_req, sizeof(struct sntp)) < 0) {
    printf("sntp: read() failed\n");
    exit(1);
  }

  // print response
  if ((uint32)ntohl(sntp_req->transmit_timestamp) == 0) {
    printf("sntp: wrong response\n");
    exit(1);
  }
  printDate((uint32)ntohl(sntp_req->transmit_timestamp));
  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc < 1) {
    fprintf(2, "Usage: sntp");
    exit(1);
  }
  sntp();
  exit(0);
}