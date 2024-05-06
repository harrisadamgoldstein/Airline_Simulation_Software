#When calling make all
all:	airport.o clock.o main.o passenger.o plane.o timeManager.o airline.o gate.o flight.o logger.o input.o finance.o
	g++ -std=c++11 -o airline_simulation airport.o clock.o main.o passenger.o plane.o timeManager.o airline.o gate.o flight.o logger.o input.o finance.o
	@echo ///////////Compilation is all green to green!//////////////

clean:
	rm *.o airline_simulation *.txt && echo All files cleaned
