TESTS = test_runner
check_PROGRAMS = $(TESTS)
test_runner_SOURCES = librepsheet_test.c   \
                      cidr_test.c          \
                      blacklist_test.c     \
                      whitelist_test.c     \
                      marked_test.c        \
                      recorder_test.c      \
                      xff_test.c           \
                      test_runner.c        \
                      ../src/librepsheet.c \
                      ../src/librepsheet.h \
                      ../src/common.c      \
                      ../src/common.h      \
                      ../src/cidr.h        \
                      ../src/cidr.c        \
                      ../src/check_cidr.h  \
                      ../src/check_cidr.c  \
                      ../src/blacklist.c   \
                      ../src/whitelist.c   \
                      ../src/marked.c      \
                      ../src/recorder.c    \
                      ../src/recorder.h    \
                      ../src/xff.c         \
                      ../src/xff.h         \
                      ../src/vector.h      \
                      ../src/vector.c
test_runner_CFLAGS = -g -O0 --coverage -std=c99 $(CHECK_CFLAGS)
test_runner_LDADD = $(CHECK_LIBS) -lm -lhiredis
