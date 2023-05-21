all: buyer.out seller.out

buyer.out: buyer.o
	gcc buyer.o -o buyer.out

seller.out: seller.o
	gcc seller.o -o seller.out	

buyer.o: buyer.c
	gcc -c buyer.c -o buyer.o

seller.o: seller.c
	gcc -c seller.c -o seller.o

clean:
	rm -f *.out
	rm -r -f *.o