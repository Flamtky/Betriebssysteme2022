#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define FILE_SIZE 10

char *expected;

int readOriginal(char *buf) {
  int fd = open("/TestFile", O_RDONLY);
  if (fd < 0) {
    printf("open failed\n");
    return -1;
  }
  int n = read(fd, buf, FILE_SIZE);
  if (n < 0) {
    printf("read failed\n");
    return -1;
  }
  close(fd);
  return 0;
}

int test(void) {
  int fd;
  char buf[10];
  int fail = 0;

  fd = open("/TestFile", O_RDONLY);
  if (fd < 0) {
    printf("Opening failed\n");
    fail = 1;
  }

  if (lseek(fd, 3, SEEK_SET) < 0) {
    printf("Test 1: lseek failed\n");
    fail = 1;
  }

  if (read(fd, buf, FILE_SIZE) < 0) {
    printf("Test 1: Read failed\n");
    fail = 1;
  }
  
  // Checks if the file was offset by 3 bytes by comparing the expected string
  for (int i = 0; i < FILE_SIZE-4; i++) {
    if (buf[i] != expected[i+3]) {
      printf("\n");
      printf("Test 1: Failed \t SEEK_SET 3\n");
      printf("%c != %c\n", buf[i], expected[i+3]);
      fail = 1;
    }
  }

  if (fail != 1) {
    printf("First Test Passed \t SEEK_SET 3\n");
  }

  // Set back to 2
  if (lseek(fd, 2, SEEK_SET) < 0) {
    printf("Test 2: lseek failed\n");
    fail = 2;
  }

  if (read(fd, buf, FILE_SIZE) < 0) {
    printf("Test 2: Read failed\n");
    fail = 2;
  }

  for (int i = 0; i < FILE_SIZE-3; i++) {
    if (buf[i] != expected[i+2]) {
      printf("\n");
      printf("Test failed \t SEEK_SET 2\n");
      printf("%c != %c\n", buf[i], expected[i+2]);
      fail = 2;
    }
  }

  if (fail != 2) {
    printf("Second Test Passed \t SEEK_SET 2\n");
  }

  // Set back to 2
  if (lseek(fd, 2, SEEK_SET) < 0) {
    printf("Test 2: lseek failed\n");
    fail = 2;
  }

  // SEEK_CUR
  if (lseek(fd, 3, SEEK_CUR) < 0) {
    printf("Test 3: lseek failed\n");
    fail = 3;
  }

  if (read(fd, buf, FILE_SIZE) < 0) {
    printf("Test 3: Read failed\n");
    fail = 3;
  }

  for (int i = 0; i < FILE_SIZE-6; i++) {
    if (buf[i] != expected[i+5]) {
      printf("\n");
      printf("Test 3: Failed \t SEEK_CUR 3\n");
      printf("%c != %c\n", buf[i], expected[i+5]);
      fail = 3;
    }
  }

  if (fail != 3) {
    printf("Third Test Passed \t SEEK_CUR 3\n");
  }

  // SEEK_END
  if (lseek(fd, -3, SEEK_END) < 0) {
    printf("Test 4: lseek failed\n");
    fail = 4;
  }

  if (read(fd, buf, FILE_SIZE) < 0) {
    printf("Test 4: Read failed\n");
    fail = 4;
  }

  for (int i = 0; i < FILE_SIZE-8; i++) {
    if (buf[i] != expected[i+7]) {
      printf("\n");
      printf("Test 4: Failed \t SEEK_END -3\n");
      printf("%c != %c\n", buf[i], expected[i+7]);
      fail = 4;
    }
  }

  if (fail != 4) {
    printf("Fourth Test Passed \t SEEK_END -3\n");
  }

  // Set at end of file
  if (lseek(fd, 10, SEEK_SET) < 0) {
    printf("Test 5: lseek failed\n");
    fail = 2;
  }

  if (lseek(fd, -2, SEEK_END) < 0) {
    printf("Test 5: lseek failed\n");
    fail = 5;
  }

  if (read(fd, buf, FILE_SIZE) < 0) {
    printf("Test 5: Read failed\n");
    fail = 5;
  }

  // Check if lseek returns -1 if the given invalid offsets

  if (lseek(fd, 1, SEEK_END) >= 0) {
    printf("\n");
    printf("Test 5: Failed \t SEEK_END -2\n");
    printf("lseek should fail with invalid offset\n");
    fail = 5;
  }

  if (fail != 5) {
    printf("Fifth Test Passed \t SEEK_END -2\n");
  }

  if (lseek(fd, -12, SEEK_END) >= 0) {
    printf("\n");
    printf("Test 6: Failed \t SEEK_END -11\n");
    printf("lseek should fail with invalid offset\n");
    fail = 6;
  }

  if (fail != 6) {
    printf("Sixth Test Passed \t SEEK_END -11\n");
  }


  if (close(fd) < 0) {
    printf("close failed\n");
    fail = 1;
  }
  return -fail;
}

int main(void) {
  int ret;
  if (readOriginal(expected) < 0) {
    fprintf(2, "readOriginal failed\n");
    exit(0);
  }
  ret = test();
  if (ret < 0) {
    fprintf(2, "Test failed\n");
  } else {
    printf("All Tests Passed\n");
  }
  exit(0);
}