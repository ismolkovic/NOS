#include "include.hpp"

class B {
	private:
		key_t msgQKey;
		int msgQID;

		void error();

	public:
		void init();
		void Start();
};

void B::error() {
	cout << "Doslo je do greske" << endl;
	exit(1);
}

void B::init() {
	// dohvacanje uid
	msgQKey = getuid();

	// dohvacenje identifikatora reda poruka
	msgQID = msgget( msgQKey, 0600 | IPC_CREAT);

	if (msgQID == -1) error();	
}

void B::Start() {
	struct msgBuffer buf;

	if (msgrcv(msgQID, &buf, sizeof(buf)-sizeof(long), 
			PROCESS_A_TO_B, 0) == -1) {
    	error();
    }

	std::cout << "....B primio od A: "<< buf.mtext << std::endl;
	
	sprintf(buf.mtext, "{E(R,Kb)}");
	buf.mtype = PROCESS_B;
	sleep(3);

	if (msgsnd(msgQID, &buf, strlen(buf.mtext)+1, 0) == -1) {
		error();
	}

	cout << "....B salje A: " << buf.mtext << endl;
		
	sleep(3);
	if (msgrcv(msgQID, &buf, sizeof(buf)-sizeof(long),
		  PROCESS_A_TO_B, 0) == -1) {
    error();
  }

	cout << "....B primio od A: "<< buf.mtext << endl;	
}

int main(int argc, const char **argv)
{
	B b;
	b.init();
	b.Start();

	return 0;
}
