#include "include.hpp"

class KDC {
	private:
		key_t msgQKey;
		void error();
	
	public:
		void init();
		void Start();
};

int msgQID = -1;

void KDC::error() {
	cout << "Doslo je do greske" << endl;
	exit(1);
}

void endSignal(int f) {
	int value = msgctl( msgQID, IPC_RMID, NULL);
	if ( value == -1 )
		exit(1);
	else 
		exit(0);
}

void KDC::init() {
	// dohvacanje uid
	msgQKey = getuid();

	// dohvacenje identifikatora reda poruka
	msgQID = msgget( msgQKey, 0600 | IPC_CREAT);

	if (msgQID == -1) error();

	// postavljenje presretanje signala
	sigset(SIGINT, endSignal);
}

void KDC::Start() {
	while(1)
	{
		struct msgBuffer buf;
		if (msgrcv(msgQID, &buf, sizeof(buf) - sizeof(long), 
				PROCESS_A, 0) == -1) {
    	error();
		}

		cout << "KDC primio: "<< buf.mtext << endl;
		sprintf(buf.mtext, "{E((IDa, Kab), Kb), E((Ra, IDa, Kab, C1), Ka)}");
		buf.mtype = PROCESS_KDC;
		sleep(3);

		if (msgsnd(msgQID, &buf, strlen(buf.mtext)+1, 0) == -1) {
			error();
		}

		cout << "KDC salje procesu A: " << buf.mtext << endl;
	}
}

int main(int argc, const char **argv)
{
	KDC kdc;
	kdc.init();
	kdc.Start();

	return 0;
}
