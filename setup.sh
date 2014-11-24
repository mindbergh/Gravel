cp -R p4grading/part1tests $1
cp -R p4grading/part1patch $1
cp -R $1/part1tests/* $1/tasks
cp -R $1/part1patch/Makefile $1/
cp -R $1/part1patch/lock.h $1/tasks/libc/include
cp -R $1/part1patch/tasks.mk $1/tasks/
cp -R $1/part1patch/*.pl $1/
