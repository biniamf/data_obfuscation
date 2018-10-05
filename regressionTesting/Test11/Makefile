CC="g++"

main= t_11

src=t_11_a.cpp t_11_b.cpp
obj=t_11_a.o t_11_b.o

all: $(main)

$(main): $(obj)
	$(CC) -o $(main) $(obj) 

%.o: %.cpp
	$(CC) -c -o $@ $<

clean:
	@rm $(main) $(obj)
