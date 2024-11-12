#ifndef CLIENT_H
#define CLIENT_H

class Client {
  public:
	Client(
		int num_restapi = 1,
		int num_ws = 1
	);
	~Client();

	void run();
};

#endif	// CLIENT_H
