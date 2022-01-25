#pragma once
#pragma warning(disable : 4996)
#include "experiments.h"
#include "RandWell.h"
#include "game.h"
#include <thread>
#include <mutex>

using namespace std;

Population *pole1_test(int gens) {
	Population *pop = 0;
	Genome *start_genome;
	char curword[20];
	int id;

	ostringstream *fnamebuf;
	int gen;

	int expcount;
	int status;
	int runs[1];
	int totalevals;
	int samples;  //For averaging

	memset(runs, 0, NEAT::num_runs * sizeof(int));

	ifstream iFile("pole1startgenes", ios::in);

	cout << "START SINGLE POLE BALANCING EVOLUTION" << endl;

	cout << "Reading in the start genome" << endl;
	//Read in the start Genome
	iFile >> curword;
	iFile >> id;
	cout << "Reading in Genome id " << id << endl;
	start_genome = new Genome(id, iFile);
	iFile.close();

	//Run multiple experiments
	for (expcount = 0; expcount < NEAT::num_runs; expcount++) {

		cout << "EXPERIMENT #" << expcount << endl;

		cout << "Start Genome: " << start_genome << endl;

		//Spawn the Population
		cout << "Spawning Population off Genome" << endl;

		pop = new Population(start_genome, NEAT::pop_size, false);

		cout << "Verifying Spawned Pop" << endl;
		pop->verify();

		for (gen = 1; gen <= gens; gen++) {
			cout << "Generation " << gen << endl;

			fnamebuf = new ostringstream();
			(*fnamebuf) << "gen_" << gen << ends;  //needs end marker

#ifndef NO_SCREEN_OUT
			cout << "name of fname: " << fnamebuf->str() << endl;
#endif	

			char temp[50];
			sprintf(temp, "gen_%d", gen);

			status = pole1_epoch(pop, gen, temp);
			//status=(pole1_epoch(pop,gen,fnamebuf->str()));

			if (status) {
				runs[expcount] = status;
				gen = gens + 1;
			}
			fnamebuf->clear();
			delete fnamebuf;
		}

		if (expcount < NEAT::num_runs - 1) delete pop;
	}

	totalevals = 0;
	samples = 0;
	for (expcount = 0; expcount < NEAT::num_runs; expcount++) {
		//cout << runs[expcount] << endl;
		if (runs[expcount] > 0)
		{
			totalevals += runs[expcount];
			samples++;
		}
	}

	cout << "Failures: " << (NEAT::num_runs - samples) << " out of " << NEAT::num_runs << " runs" << endl;
	cout << "Average evals: " << (samples > 0 ? (double)totalevals / samples : 0) << endl;

	return pop;

}

int pole1_epoch(Population *pop, int generation, char *filename) {
	vector<Organism*>::iterator curorg;
	vector<Species*>::iterator curspecies;
	//char cfilename[100];
	//strncpy( cfilename, filename.c_str(), 100 );

	//ofstream cfilename(filename.c_str());
	bool win = false;
	int winnernum;

	//Evaluate each organism on a test
	for (curorg = (pop->organisms).begin(); curorg != (pop->organisms).end(); ++curorg) {
		if (pole1_evaluate(*curorg)) win = true;
	}

	//Average and max their fitnesses for dumping to file and snapshot
	for (curspecies = (pop->species).begin(); curspecies != (pop->species).end(); ++curspecies) {
		//This experiment control routine issues commands to collect ave
		//and max fitness, as opposed to having the snapshot do it, 
		//because this allows flexibility in terms of what time
		//to observe fitnesses at

		(*curspecies)->compute_average_fitness();
		(*curspecies)->compute_max_fitness();
	}

	//Take a snapshot of the population, so that it can be
	//visualized later on
	//if ((generation%1)==0)
	//  pop->snapshot();

	//Only print to file every print_every generations
	if (win || ((generation % (NEAT::print_every)) == 0))
		pop->print_to_file_by_species(filename);

	if (win) {
		for (curorg = (pop->organisms).begin(); curorg != (pop->organisms).end(); ++curorg) {
			if ((*curorg)->winner) {
				winnernum = ((*curorg)->gnome)->genome_id;
				cout << "WINNER IS #" << ((*curorg)->gnome)->genome_id << endl;
			}
		}
	}

	//Create the next generation
	pop->epoch(generation);

	if (win) return ((generation - 1)*NEAT::pop_size + winnernum);
	else return 0;

}

bool pole1_evaluate(Organism *org) {
	Network *net;

	int numnodes;  /* Used to figure out how many nodes
			  should be visited during activation */
	int thresh;  /* How many visits will be allowed before giving up
			(for loop detection) */

			//  int MAX_STEPS=120000;
	int MAX_STEPS = 100000;

	net = org->net;

	numnodes = ((org->gnome)->nodes).size();
	thresh = numnodes * 2;  //Max number of visits allowed per activation

	//Try to balance a pole now
	org->fitness = go_cart(net, MAX_STEPS, thresh);

#ifndef NO_SCREEN_OUT
	cout << "Org " << (org->gnome)->genome_id << " fitness: " << org->fitness << endl;
#endif

	//Decide if its a winner
	if (org->fitness >= MAX_STEPS) {
		org->winner = true;
		return true;
	}
	else {
		org->winner = false;
		return false;
	}

}

//     cart_and_pole() was take directly from the pole simulator written
//     by Richard Sutton and Charles Anderson.
int go_cart(Network *net, int max_steps, int thresh)
{
	float x,			/* cart position, meters */
		x_dot,			/* cart velocity */
		theta,			/* pole angle, radians */
		theta_dot;		/* pole angular velocity */
	int steps = 0, y;

	int random_start = 1;

	double in[5];  //Input loading array

	double out1;
	double out2;

	//     double one_degree= 0.0174532;	/* 2pi/360 */
	//     double six_degrees=0.1047192;
	double twelve_degrees = 0.2094384;
	//     double thirty_six_degrees= 0.628329;
	//     double fifty_degrees=0.87266;

	vector<NNode*>::iterator out_iter;

	if (random_start) {
		/*set up random start state*/
		x = (randbtn(0, 2147483647) % 4800) / 1000.0 - 2.4;
		x_dot = (randbtn(0, 2147483647) % 2000) / 1000.0 - 1;
		theta = (randbtn(0, 2147483647) % 400) / 1000.0 - 0.2;
		theta_dot = (randbtn(0, 2147483647) % 3000) / 1000.0 - 1.5;
	}
	else
		x = x_dot = theta = theta_dot = 0.0;

	/*--- Iterate through the action-learn loop. ---*/
	while (steps++ < max_steps)
	{

		/*-- setup the input layer based on the four iputs --*/
		//setup_input(net,x,x_dot,theta,theta_dot);
		in[0] = 1.0;  //Bias
		in[1] = (x + 2.4) / 4.8;;
		in[2] = (x_dot + .75) / 1.5;
		in[3] = (theta + twelve_degrees) / .41;
		in[4] = (theta_dot + 1.0) / 2.0;
		net->load_sensors(in);

		//activate_net(net);   /*-- activate the network based on the input --*/
		//Activate the net
		//If it loops, exit returning only fitness of 1 step
		if (!(net->activate())) return 1;

		/*-- decide which way to push via which output unit is greater --*/
		out_iter = net->outputs.begin();
		out1 = (*out_iter)->activation;
		++out_iter;
		out2 = (*out_iter)->activation;
		if (out1 > out2)
			y = 0;
		else
			y = 1;

		/*--- Apply action to the simulated cart-pole ---*/
		cart_pole(y, &x, &x_dot, &theta, &theta_dot);
		/*--- Check for failure.  If so, return steps ---*/
		if (x < -2.4 || x > 2.4 || theta < -twelve_degrees || theta > twelve_degrees)
			return steps;
	}
	
	return steps;
}


//     cart_and_pole() was take directly from the pole simulator written
//     by Richard Sutton and Charles Anderson.
//     This simulator uses normalized, continous inputs instead of 
//    discretizing the input space.
/*----------------------------------------------------------------------
   cart_pole:  Takes an action (0 or 1) and the current values of the
 four state variables and updates their values by estimating the state
 TAU seconds later.
----------------------------------------------------------------------*/
void cart_pole(int action, float *x, float *x_dot, float *theta, float *theta_dot) {
	float xacc, thetaacc, force, costheta, sintheta, temp;

	const float GRAVITY = 9.8;
	const float MASSCART = 1.0;
	const float MASSPOLE = 0.1;
	const float TOTAL_MASS = (MASSPOLE + MASSCART);
	const float LENGTH = 0.5;	  /* actually half the pole's length */
	const float POLEMASS_LENGTH = (MASSPOLE * LENGTH);
	const float FORCE_MAG = 10.0;
	const float TAU = 0.02;	  /* seconds between state updates */
	const float FOURTHIRDS = 1.3333333333333;

	force = (action > 0) ? FORCE_MAG : -FORCE_MAG;
	costheta = cos(*theta);
	sintheta = sin(*theta);

	temp = (force + POLEMASS_LENGTH * *theta_dot * *theta_dot * sintheta)
		/ TOTAL_MASS;

	thetaacc = (GRAVITY * sintheta - costheta * temp)
		/ (LENGTH * (FOURTHIRDS - MASSPOLE * costheta * costheta
			/ TOTAL_MASS));

	xacc = temp - POLEMASS_LENGTH * thetaacc* costheta / TOTAL_MASS;

	/*** Update the four state variables, using Euler's method. ***/

	*x += TAU * *x_dot;
	*x_dot += TAU * xacc;
	*theta += TAU * *theta_dot;
	*theta_dot += TAU * thetaacc;
}

Population *tank_game(int gens)
{
	Population *pop = 0;
	Genome *start_genome;
	Genome *second_genome;
	char curword[20];
	int id;

	ostringstream *fnamebuf;
	int gen;

	int expcount;
	int status;
	int runs[1];
	int totalevals;
	int samples;  //For averaging
	bool isFinished;

	memset(runs, 0, NEAT::num_runs * sizeof(int));

	ifstream iFile("tankgenes", ios::in);
	ifstream inputFile("tankGen", ios::in);
	ifstream secondFile("tank2Gen", ios::in);

	cout << "START SINGLE POLE BALANCING EVOLUTION" << endl;
	
	if (!inputFile)
	{
		cout << "there is not file" << endl;

		cout << "Reading in the start genome" << endl;
		//Read in the start Genome
		iFile >> curword;
		iFile >> id;
		cout << "Reading in Genome id " << id << endl;
		start_genome = new Genome(id, iFile);
		iFile.close();
		isFinished = false;

		//Run multiple experiments
		for (expcount = 0; expcount < NEAT::num_runs; expcount++) {

			cout << "EXPERIMENT #" << expcount << endl;

			cout << "Start Genome: " << start_genome << endl;

			//Spawn the Population
			cout << "Spawning Population off Genome" << endl;

			pop = new Population(start_genome, NEAT::pop_size, isFinished);

			cout << "Verifying Spawned Pop" << endl;
			pop->verify();

			for (gen = 1; gen <= gens; gen++) {
				cout << "Generation " << gen << endl;

				fnamebuf = new ostringstream();
				(*fnamebuf) << "gen_" << gen << ends;  //needs end marker

				char temp[50];
				sprintf(temp, "gen_%d", gen);

				//status = pole1_epoch(pop, gen, temp);
				//status=(pole1_epoch(pop,gen,fnamebuf->str()));
				status = measure_fitness_tank(pop, gen, temp);

				if (status) {
					runs[expcount] = status;
					gen = gens + 1;
				}
				fnamebuf->clear();
				delete fnamebuf;
			}

			if (expcount < NEAT::num_runs - 1) delete pop;
		}
	}
	else
	{
		inputFile >> curword;
		inputFile >> id;
		start_genome = new Genome(id, inputFile);
		inputFile.close();

		secondFile >> curword;
		secondFile >> id;
		second_genome = new Genome(id, secondFile);
		secondFile.close();
		isFinished = true;

		for (expcount = 0; expcount < NEAT::num_runs; expcount++) {

			cout << "EXPERIMENT #" << expcount << endl;

			cout << "Start Genome: " << start_genome << endl;

			//Spawn the Population
			cout << "Spawning Population off Genome" << endl;

			pop = new Population(start_genome, NEAT::pop_size, isFinished);
			Organism *new_organism;
			for (int i = 0; i < pop->organisms.size(); i++)
			{
				if (i % 2 == 1)
				{
					Genome *new_genome = nullptr;
					new_genome = second_genome->duplicate(i);
					new_organism = new Organism(0.0, new_genome, 1);
					pop->organisms[i] = new_organism;
				}
			}

			cout << "Verifying Spawned Pop" << endl;
			pop->verify();

			for (gen = 1; gen <= gens; gen++) {
				cout << "Generation " << gen << endl;

				fnamebuf = new ostringstream();
				(*fnamebuf) << "gen_" << gen << ends;  //needs end marker

				char temp[50];
				sprintf(temp, "gen_%d", gen);

				//status = pole1_epoch(pop, gen, temp);
				//status=(pole1_epoch(pop,gen,fnamebuf->str()));
				status = measure_fitness_tank(pop, gen, temp);

				if (status) {
					runs[expcount] = status;
					gen = gens + 1;
				}
				fnamebuf->clear();
				delete fnamebuf;
			}

			if (expcount < NEAT::num_runs - 1) delete pop;
		}
	}

	totalevals = 0;
	samples = 0;
	for (expcount = 0; expcount < NEAT::num_runs; expcount++) {
		//cout << runs[expcount] << endl;
		if (runs[expcount] > 0)
		{
			totalevals += runs[expcount];
			samples++;
		}
	}

	cout << "Failures: " << (NEAT::num_runs - samples) << " out of " << NEAT::num_runs << " runs" << endl;
	cout << "Average evals: " << (samples > 0 ? (double)totalevals / samples : 0) << endl;

	return pop;
}

bool tank_evaluate(Organism * org, bool &isWin, int num)
{
	Network *net;

	int numnodes;  /* Used to figure out how many nodes
			  should be visited during activation */
	int thresh;  /* How many visits will be allowed before giving up
			(for loop detection) */

			//  int MAX_STEPS=120000;
	int MAX_STEPS = 100000;

	if (org)
		net = org->net;
	else
		return false;

	numnodes = ((org->gnome)->nodes).size();
	thresh = numnodes * 2;  //Max number of visits allowed per activation

	//Try to balance a pole now

	if (net->net_id > 0)
	{
		//org->fitness = try_tank(net, MAX_STEPS, thresh, num);
		if (num < 3)
			user_fitness_sum += try_tank(net, MAX_STEPS, thresh, num);
		else
			enemy_fitness_sum += try_tank(net, MAX_STEPS, thresh, num);
		//cout << org->fitness << endl;
	}
	else
	{
		isWin = false;
		return false;
	}

	//Decide if its a winner
	if (org->fitness >= MAX_STEPS) {
		org->winner = true;
		isWin = true;
		return true;
	}
	else {
		org->winner = false;
		isWin = false;
		return false;
	}
}

int measure_fitness_tank(Population * pop, int generation, char * filename)
{
	vector<Organism*>::iterator curorg;
	vector<Species*>::iterator curspecies;

	//ofstream cfilename(filename.c_str());

	bool win = false;
	int winnernum = 0;

	//Evaluate each organism on a test
	/*for (curorg = (pop->organisms).begin(); curorg != (pop->organisms).end(); ++curorg) {
		if (flappybird_evaluate(*curorg)) win = true;
	}*/
	vector<thread> thread_pool;

	int count = 0;
	while (count < pop->organisms.size())
	{
		if (startNextGame)
		{
			startNextGame = false;
			int num = 0;

			pop->organisms[count]->Users.push_back(initial_pos[0]);
			pop->organisms[count]->Users.push_back(initial_pos[1]);
			pop->organisms[count]->Users.push_back(initial_pos[2]);
			pop->organisms[count]->Users.push_back(initial_pos[3]);
			pop->organisms[count]->Users.push_back(initial_pos[4]);
			pop->organisms[count]->Users.push_back(initial_pos[5]);
			pop->organisms[count]->Enemies.push_back(initial_pos[6]);
			pop->organisms[count]->Enemies.push_back(initial_pos[7]);
			pop->organisms[count]->Enemies.push_back(initial_pos[8]);
			pop->organisms[count]->Enemies.push_back(initial_pos[9]);
			pop->organisms[count]->Enemies.push_back(initial_pos[10]);
			pop->organisms[count]->Enemies.push_back(initial_pos[11]);

			for (auto &i : users)
			{
				thread_pool.emplace_back(thread(&tank_evaluate, pop->organisms[count], ref(win), num));
				num++;
			}
			count++;

			pop->organisms[count]->Users.push_back(initial_pos[0]);
			pop->organisms[count]->Users.push_back(initial_pos[1]);
			pop->organisms[count]->Users.push_back(initial_pos[2]);
			pop->organisms[count]->Users.push_back(initial_pos[3]);
			pop->organisms[count]->Users.push_back(initial_pos[4]);
			pop->organisms[count]->Users.push_back(initial_pos[5]);
			pop->organisms[count]->Enemies.push_back(initial_pos[6]);
			pop->organisms[count]->Enemies.push_back(initial_pos[7]);
			pop->organisms[count]->Enemies.push_back(initial_pos[8]);
			pop->organisms[count]->Enemies.push_back(initial_pos[9]);
			pop->organisms[count]->Enemies.push_back(initial_pos[10]);
			pop->organisms[count]->Enemies.push_back(initial_pos[11]);

			for (auto &i : enemies)
			{
				thread_pool.emplace_back(thread(&tank_evaluate, pop->organisms[count], ref(win), num));
				num++;
			}
			count++;

			for (auto &i : thread_pool)
			{
				//cout << "here" << endl;
				i.join();
			}

			joinFinish = true;
			//cout << "join!" << endl;
			pop->organisms[count - 2]->fitness = user_fitness_sum;
			pop->organisms[count - 1]->fitness = enemy_fitness_sum;

			//각 게임에 대한 fitness를 측정하고 여기서 더함. fitness측정은 어떻게?
			//우선..게임 끝났을 때의 상태를 저장하고 해당 정보를 바탕으로
			//탱크 몇 마리가 데미지를 얼만큼 데미지를 입었는지..

			user_fitness_sum = 0;
			enemy_fitness_sum = 0;

			thread_pool.clear();
		}
	}

	//for (auto &thread : thread_pool)
	//	thread.join();

	//Average and max their fitnesses for dumping to file and snapshot
	for (curspecies = (pop->species).begin(); curspecies != (pop->species).end(); ++curspecies) {
		//This experiment control routine issues commands to collect ave
		//and max fitness, as opposed to having the snapshot do it, 
		//because this allows flexibility in terms of what time
		//to observe fitnesses at

		(*curspecies)->compute_average_fitness();
		(*curspecies)->compute_max_fitness();
	}

	//Only print to file every print_every generations
	if (win || ((generation % (NEAT::print_every)) == 0))
		pop->print_to_file_by_species(filename);

	if (win) {
		for (curorg = (pop->organisms).begin(); curorg != (pop->organisms).end(); ++curorg) {
			if ((*curorg)->winner) {
				winnernum = ((*curorg)->gnome)->genome_id;
				cout << "WINNER IS #" << ((*curorg)->gnome)->genome_id << endl;
			}
		}
	}
	//Create the next generation
	pop->epoch(generation);

	if (win) return ((generation - 1)*NEAT::pop_size + winnernum);
	else return 0;
}

int try_tank(Network * net, int max_steps, int thresh, int num)
{
	//Measure Fitness
	//Input Value
	//1. 자신의 x 위치
	//2. 자신의 y 위치
	//3. 팀원1의 x 위치
	//4. 팀원1의 y 위치
	//5. 팀원2의 x 위치
	//6. 팀원2의 y 위치
	//7. 적1의 x 위치
	//8. 적1의 y 위치
	//9. 적2의 x 위치
	//10. 적2의 y 위치
	//11. 적3의 x 위치
	//12. 적3의 y 위치
	//13. 자신의 포의 각도
	
	//Output Value
	//1. 왼쪽 바퀴 가속도
	//2. 오른쪽 바퀴 가속도
	//3. 쏠지 안쏠지
	//4. 포 움직일 각도

	vector<NNode*>::iterator out_iter;
	float fitness = 0;

	int random = 0;

	while (GameOver)
	{

	}

	if (num < 3)
	{
		users[num]->isDie = false;
		users[num]->hp = 100;
		users[num]->angle = 90;
		users[num]->c_angle = 0.0f;
	}
	else
	{
		enemies[num - 3]->isDie = false;
		enemies[num - 3]->hp = 100;
		enemies[num - 3]->angle = 270;
		enemies[num - 3]->c_angle = 0.0f;
	}

	while (!GameOver) //다음으로 넘어갈 조건..->게임오버
	{
		//Measure만 넣으면 됨!!!
		mutex_2.lock();
		double out_Left;
		double out_Right;
		double out_isShoot;
		double out_angle;
		double in[13];

		if (num < 3)
		{
			//User(아래쪽)
			in[0] = users[num]->x;
			in[1] = users[num]->y;
			if (num == 0)
			{
				in[2] = users[num + 1]->x;
				in[3] = users[num + 1]->y;
				in[4] = users[num + 2]->x;
				in[5] = users[num + 2]->y;
			}
			else if (num == 1)
			{
				in[2] = users[num - 1]->x;
				in[3] = users[num - 1]->y;
				in[4] = users[num + 1]->x;
				in[5] = users[num + 1]->y;
			}
			else
			{
				in[2] = users[num - 2]->x;
				in[3] = users[num - 2]->y;
				in[4] = users[num - 1]->x;
				in[5] = users[num - 1]->y;
			}

			in[6] = enemies[0]->x;
			in[7] = enemies[0]->y;
			in[8] = enemies[1]->x;
			in[9] = enemies[1]->y;
			in[10] = enemies[2]->x;
			in[11] = enemies[2]->y;
			in[12] = -(users[num]->angle - 90 + users[num]->c_angle * 180 / 3.14159);
		}
		else
		{
			in[0] = enemies[num - 3]->x;
			in[1] = enemies[num - 3]->y;
			if (num == 3)
			{
				in[2] = enemies[num - 2]->x;
				in[3] = enemies[num - 2]->y;
				in[4] = enemies[num - 1]->x;
				in[5] = enemies[num - 1]->y;
			}
			else if (num == 4)
			{
				in[2] = enemies[num - 4]->x;
				in[3] = enemies[num - 4]->y;
				in[4] = enemies[num - 2]->x;
				in[5] = enemies[num - 2]->y;
			}
			else
			{
				in[2] = enemies[num - 5]->x;
				in[3] = enemies[num - 5]->y;
				in[4] = enemies[num - 4]->x;
				in[5] = enemies[num - 4]->y;
			}
			in[6] = users[0]->x;
			in[7] = users[0]->y;
			in[8] = users[1]->x;
			in[9] = users[1]->y;
			in[10] = users[2]->x;
			in[11] = users[2]->y;
			in[12] = -(enemies[num - 3]->angle - 90 + enemies[num - 3]->c_angle * 180 / 3.14159);
		}

		net->load_sensors(in);

		if (!(net->activate()))
			return 1;

		out_iter = net->outputs.begin();
		out_Left = (*out_iter)->activation;
		++out_iter;
		out_Right = (*out_iter)->activation;
		++out_iter;
		out_isShoot = (*out_iter)->activation;
		++out_iter;
		out_angle = (*out_iter)->activation;

		if (num < 3)
		{
			users[num]->MoveUser(out_Left / 1000000, out_Right / 1000000);
			users[num]->RotateCannon(out_angle);
			if (out_isShoot > 0)
				users[num]->gun->Shoot();
		}
		else
		{
			enemies[num - 3]->MoveUser(out_Left / 1000000, out_Right / 1000000);
			enemies[num - 3]->RotateCannon(out_angle);
			if (out_isShoot > 0)
				enemies[num - 3]->gun->Shoot();
		}
		mutex_2.unlock();
		Sleep(50);
		random = randbtn(1, 10000);
	}

	return random;
}
