#include "include.hpp"

class A {
	private:
		key_t msgQKey;
		int msgQID;

		void error();

	public:
		void init();
		void Start();
};

void A::error() {
	cout << "Doslo je do greske" << endl;
	exit(1);
}

void A::init() {
	// dohvacanje uid
	msgQKey = getuid();

	// dohvacenje identifikatora reda poruka
	msgQID = msgget( msgQKey, 0600 | IPC_CREAT);

	if (msgQID == -1) error();
}

void A::Start() {
	struct msgBuffer buf;

	sprintf(buf.mtext, "{RA, IDA, IDB}");
	buf.mtype = PROCESS_A;

	if (msgsnd(msgQID, &buf, strlen(buf.mtext)+1, 0) == -1) {
		error();
	}

	cout << "..A salje KDC-u: " << buf.mtext << endl;

	if (msgrcv(msgQID, &buf, sizeof(buf)-sizeof(long), 
			PROCESS_KDC, 0) == -1) {
    error();
   }

	sleep(3);
	cout << "..A primio od KDC-a: "<< buf.mtext << endl;

	sprintf(buf.mtext, "{C1}");
	buf.mtype = PROCESS_A_TO_B;
	if (msgsnd(msgQID, &buf, strlen(buf.mtext)+1, 0) == -1) {
		error();
	}

	std::cout << "..A salje procesu B: " << buf.mtext << std::endl;

	if (msgrcv(msgQID, &buf, sizeof(buf)-sizeof(long), 
			PROCESS_B, 0) == -1) {
    error();
   }
		
	cout << "..A primio od procesa B: "<< buf.mtext << endl;
	sleep(3);

	sprintf(buf.mtext, "{E(F(R), Kab)}");
	buf.mtype = PROCESS_A_TO_B;
	if (msgsnd(msgQID, &buf, strlen(buf.mtext)+1, 0) == -1) {
		error();
	}

	cout << "..A salje procesu B: "<< buf.mtext << endl;
	sleep(3);
}

int main(int argc, const char **argv)
{
	A a;
	a.init();
	a.Start();

	return 0;
}
