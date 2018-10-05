CC="g++"

main= md5main

src=md5.c md5main.c
obj=md5.o md5main.o

all: $(main)

$(main): $(obj)
	$(CC) -o $(main) $(obj) 

%.o: %.c
	$(CC) -c -o $@ $<

clean:
	@rm $(main) $(obj)
