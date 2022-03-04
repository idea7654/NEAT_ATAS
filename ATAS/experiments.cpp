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

	ifstream iFile("tankgenes_mirror", ios::in);
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

bool tank_evaluate(Organism * org, bool &isWin, int num, int roomNum)
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
		switch (roomNum)
		{
		case 0:
		{
			if (num < 3)
				user_fitness_sum += try_tank_mirror(net, MAX_STEPS, thresh, num, roomNum);
				//user_fitness_sum += try_tank(net, MAX_STEPS, thresh, num, roomNum);
			else
				enemy_fitness_sum += try_tank_mirror(net, MAX_STEPS, thresh, num, roomNum);
				//enemy_fitness_sum += try_tank(net, MAX_STEPS, thresh, num, roomNum);
			break;
		}
		case 1:
		{
			if (num < 3)
				user_fitness_sum2 += try_tank_mirror(net, MAX_STEPS, thresh, num, roomNum);
			else
				enemy_fitness_sum2 += try_tank_mirror(net, MAX_STEPS, thresh, num, roomNum);
			break;
		}
		case 2:
		{
			if (num < 3)
				user_fitness_sum3 += try_tank_mirror(net, MAX_STEPS, thresh, num, roomNum);
			else
				enemy_fitness_sum3 += try_tank_mirror(net, MAX_STEPS, thresh, num, roomNum);
			break;
		}
		case 3:
		{
			if (num < 3)
				user_fitness_sum4 += try_tank_mirror(net, MAX_STEPS, thresh, num, roomNum);
			else
				enemy_fitness_sum4 += try_tank_mirror(net, MAX_STEPS, thresh, num, roomNum);
			break;
		}
		case 4:
		{
			if (num < 3)
				user_fitness_sum5 += try_tank_mirror(net, MAX_STEPS, thresh, num, roomNum);
			else
				enemy_fitness_sum5 += try_tank_mirror(net, MAX_STEPS, thresh, num, roomNum);
			break;
		}
		case 5:
		{
			if (num < 3)
				user_fitness_sum6 += try_tank_mirror(net, MAX_STEPS, thresh, num, roomNum);
			else
				enemy_fitness_sum6 += try_tank_mirror(net, MAX_STEPS, thresh, num, roomNum);
			break;
		}
		case 6:
		{
			if (num < 3)
				user_fitness_sum7 += try_tank_mirror(net, MAX_STEPS, thresh, num, roomNum);
			else
				enemy_fitness_sum7 += try_tank_mirror(net, MAX_STEPS, thresh, num, roomNum);
			break;
		}
		case 7:
		{
			if (num < 3)
				user_fitness_sum8 += try_tank_mirror(net, MAX_STEPS, thresh, num, roomNum);
			else
				enemy_fitness_sum8 += try_tank_mirror(net, MAX_STEPS, thresh, num, roomNum);
			break;
		}
		default:
			break;
		}
		
		//cout << org->fitness << endl;
	}
	else
	{
		net->isFailed = true;
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
		int num = 0;
		int roomNum = 0;

		if (startNextGame)
		{
			startNextGame = false;

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
				thread_pool.emplace_back(thread(&tank_evaluate, pop->organisms[count], ref(win), num, roomNum));
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
				thread_pool.emplace_back(thread(&tank_evaluate, pop->organisms[count], ref(win), num, roomNum));
				num++;
			}
			count++;
		}
		if (startNextGame2)
		{
			startNextGame2 = false;
			//----------- room2 ----------
			num = 0;
			roomNum++;

			pop->organisms[count]->Users.push_back(initial_pos2[0]);
			pop->organisms[count]->Users.push_back(initial_pos2[1]);
			pop->organisms[count]->Users.push_back(initial_pos2[2]);
			pop->organisms[count]->Users.push_back(initial_pos2[3]);
			pop->organisms[count]->Users.push_back(initial_pos2[4]);
			pop->organisms[count]->Users.push_back(initial_pos2[5]);
			pop->organisms[count]->Enemies.push_back(initial_pos2[6]);
			pop->organisms[count]->Enemies.push_back(initial_pos2[7]);
			pop->organisms[count]->Enemies.push_back(initial_pos2[8]);
			pop->organisms[count]->Enemies.push_back(initial_pos2[9]);
			pop->organisms[count]->Enemies.push_back(initial_pos2[10]);
			pop->organisms[count]->Enemies.push_back(initial_pos2[11]);

			for (auto &i : users2)
			{
				thread_pool.emplace_back(thread(&tank_evaluate, pop->organisms[count], ref(win), num, roomNum));
				num++;
			}
			count++;

			pop->organisms[count]->Users.push_back(initial_pos2[0]);
			pop->organisms[count]->Users.push_back(initial_pos2[1]);
			pop->organisms[count]->Users.push_back(initial_pos2[2]);
			pop->organisms[count]->Users.push_back(initial_pos2[3]);
			pop->organisms[count]->Users.push_back(initial_pos2[4]);
			pop->organisms[count]->Users.push_back(initial_pos2[5]);
			pop->organisms[count]->Enemies.push_back(initial_pos2[6]);
			pop->organisms[count]->Enemies.push_back(initial_pos2[7]);
			pop->organisms[count]->Enemies.push_back(initial_pos2[8]);
			pop->organisms[count]->Enemies.push_back(initial_pos2[9]);
			pop->organisms[count]->Enemies.push_back(initial_pos2[10]);
			pop->organisms[count]->Enemies.push_back(initial_pos2[11]);

			for (auto &i : enemies2)
			{
				thread_pool.emplace_back(thread(&tank_evaluate, pop->organisms[count], ref(win), num, roomNum));
				num++;
			}
			count++;

			//-----------------
		}
		if (startNextGame3)
		{
			startNextGame3 = true;
			//----------- room3 ----------
			num = 0;
			roomNum++;

			pop->organisms[count]->Users.push_back(initial_pos3[0]);
			pop->organisms[count]->Users.push_back(initial_pos3[1]);
			pop->organisms[count]->Users.push_back(initial_pos3[2]);
			pop->organisms[count]->Users.push_back(initial_pos3[3]);
			pop->organisms[count]->Users.push_back(initial_pos3[4]);
			pop->organisms[count]->Users.push_back(initial_pos3[5]);
			pop->organisms[count]->Enemies.push_back(initial_pos3[6]);
			pop->organisms[count]->Enemies.push_back(initial_pos3[7]);
			pop->organisms[count]->Enemies.push_back(initial_pos3[8]);
			pop->organisms[count]->Enemies.push_back(initial_pos3[9]);
			pop->organisms[count]->Enemies.push_back(initial_pos3[10]);
			pop->organisms[count]->Enemies.push_back(initial_pos3[11]);

			for (auto &i : users3)
			{
				thread_pool.emplace_back(thread(&tank_evaluate, pop->organisms[count], ref(win), num, roomNum));
				num++;
			}
			count++;

			pop->organisms[count]->Users.push_back(initial_pos3[0]);
			pop->organisms[count]->Users.push_back(initial_pos3[1]);
			pop->organisms[count]->Users.push_back(initial_pos3[2]);
			pop->organisms[count]->Users.push_back(initial_pos3[3]);
			pop->organisms[count]->Users.push_back(initial_pos3[4]);
			pop->organisms[count]->Users.push_back(initial_pos3[5]);
			pop->organisms[count]->Enemies.push_back(initial_pos3[6]);
			pop->organisms[count]->Enemies.push_back(initial_pos3[7]);
			pop->organisms[count]->Enemies.push_back(initial_pos3[8]);
			pop->organisms[count]->Enemies.push_back(initial_pos3[9]);
			pop->organisms[count]->Enemies.push_back(initial_pos3[10]);
			pop->organisms[count]->Enemies.push_back(initial_pos3[11]);

			for (auto &i : enemies3)
			{
				thread_pool.emplace_back(thread(&tank_evaluate, pop->organisms[count], ref(win), num, roomNum));
				num++;
			}
			count++;

			//-----------------
		}
		if (startNextGame4)
		{
			startNextGame4 = false;
			//----------- room4 ----------
			num = 0;
			roomNum++;

			pop->organisms[count]->Users.push_back(initial_pos4[0]);
			pop->organisms[count]->Users.push_back(initial_pos4[1]);
			pop->organisms[count]->Users.push_back(initial_pos4[2]);
			pop->organisms[count]->Users.push_back(initial_pos4[3]);
			pop->organisms[count]->Users.push_back(initial_pos4[4]);
			pop->organisms[count]->Users.push_back(initial_pos4[5]);
			pop->organisms[count]->Enemies.push_back(initial_pos4[6]);
			pop->organisms[count]->Enemies.push_back(initial_pos4[7]);
			pop->organisms[count]->Enemies.push_back(initial_pos4[8]);
			pop->organisms[count]->Enemies.push_back(initial_pos4[9]);
			pop->organisms[count]->Enemies.push_back(initial_pos4[10]);
			pop->organisms[count]->Enemies.push_back(initial_pos4[11]);

			for (auto &i : users4)
			{
				thread_pool.emplace_back(thread(&tank_evaluate, pop->organisms[count], ref(win), num, roomNum));
				num++;
			}
			count++;

			pop->organisms[count]->Users.push_back(initial_pos4[0]);
			pop->organisms[count]->Users.push_back(initial_pos4[1]);
			pop->organisms[count]->Users.push_back(initial_pos4[2]);
			pop->organisms[count]->Users.push_back(initial_pos4[3]);
			pop->organisms[count]->Users.push_back(initial_pos4[4]);
			pop->organisms[count]->Users.push_back(initial_pos4[5]);
			pop->organisms[count]->Enemies.push_back(initial_pos4[6]);
			pop->organisms[count]->Enemies.push_back(initial_pos4[7]);
			pop->organisms[count]->Enemies.push_back(initial_pos4[8]);
			pop->organisms[count]->Enemies.push_back(initial_pos4[9]);
			pop->organisms[count]->Enemies.push_back(initial_pos4[10]);
			pop->organisms[count]->Enemies.push_back(initial_pos4[11]);

			for (auto &i : enemies4)
			{
				thread_pool.emplace_back(thread(&tank_evaluate, pop->organisms[count], ref(win), num, roomNum));
				num++;
			}
			count++;

			//-----------------
		}
		if (startNextGame5)
		{
			startNextGame5 = false;
			//----------- room5 ----------
			num = 0;
			roomNum++;

			pop->organisms[count]->Users.push_back(initial_pos5[0]);
			pop->organisms[count]->Users.push_back(initial_pos5[1]);
			pop->organisms[count]->Users.push_back(initial_pos5[2]);
			pop->organisms[count]->Users.push_back(initial_pos5[3]);
			pop->organisms[count]->Users.push_back(initial_pos5[4]);
			pop->organisms[count]->Users.push_back(initial_pos5[5]);
			pop->organisms[count]->Enemies.push_back(initial_pos5[6]);
			pop->organisms[count]->Enemies.push_back(initial_pos5[7]);
			pop->organisms[count]->Enemies.push_back(initial_pos5[8]);
			pop->organisms[count]->Enemies.push_back(initial_pos5[9]);
			pop->organisms[count]->Enemies.push_back(initial_pos5[10]);
			pop->organisms[count]->Enemies.push_back(initial_pos5[11]);

			for (auto &i : users5)
			{
				thread_pool.emplace_back(thread(&tank_evaluate, pop->organisms[count], ref(win), num, roomNum));
				num++;
			}
			count++;

			pop->organisms[count]->Users.push_back(initial_pos5[0]);
			pop->organisms[count]->Users.push_back(initial_pos5[1]);
			pop->organisms[count]->Users.push_back(initial_pos5[2]);
			pop->organisms[count]->Users.push_back(initial_pos5[3]);
			pop->organisms[count]->Users.push_back(initial_pos5[4]);
			pop->organisms[count]->Users.push_back(initial_pos5[5]);
			pop->organisms[count]->Enemies.push_back(initial_pos5[6]);
			pop->organisms[count]->Enemies.push_back(initial_pos5[7]);
			pop->organisms[count]->Enemies.push_back(initial_pos5[8]);
			pop->organisms[count]->Enemies.push_back(initial_pos5[9]);
			pop->organisms[count]->Enemies.push_back(initial_pos5[10]);
			pop->organisms[count]->Enemies.push_back(initial_pos5[11]);

			for (auto &i : enemies5)
			{
				thread_pool.emplace_back(thread(&tank_evaluate, pop->organisms[count], ref(win), num, roomNum));
				num++;
			}
			count++;

			//-----------------
		}
		if (startNextGame6)
		{
			startNextGame6 = false;
			//----------- room6 ----------
			num = 0;
			roomNum++;

			pop->organisms[count]->Users.push_back(initial_pos6[0]);
			pop->organisms[count]->Users.push_back(initial_pos6[1]);
			pop->organisms[count]->Users.push_back(initial_pos6[2]);
			pop->organisms[count]->Users.push_back(initial_pos6[3]);
			pop->organisms[count]->Users.push_back(initial_pos6[4]);
			pop->organisms[count]->Users.push_back(initial_pos6[5]);
			pop->organisms[count]->Enemies.push_back(initial_pos6[6]);
			pop->organisms[count]->Enemies.push_back(initial_pos6[7]);
			pop->organisms[count]->Enemies.push_back(initial_pos6[8]);
			pop->organisms[count]->Enemies.push_back(initial_pos6[9]);
			pop->organisms[count]->Enemies.push_back(initial_pos6[10]);
			pop->organisms[count]->Enemies.push_back(initial_pos6[11]);

			for (auto &i : users6)
			{
				thread_pool.emplace_back(thread(&tank_evaluate, pop->organisms[count], ref(win), num, roomNum));
				num++;
			}
			count++;

			pop->organisms[count]->Users.push_back(initial_pos6[0]);
			pop->organisms[count]->Users.push_back(initial_pos6[1]);
			pop->organisms[count]->Users.push_back(initial_pos6[2]);
			pop->organisms[count]->Users.push_back(initial_pos6[3]);
			pop->organisms[count]->Users.push_back(initial_pos6[4]);
			pop->organisms[count]->Users.push_back(initial_pos6[5]);
			pop->organisms[count]->Enemies.push_back(initial_pos6[6]);
			pop->organisms[count]->Enemies.push_back(initial_pos6[7]);
			pop->organisms[count]->Enemies.push_back(initial_pos6[8]);
			pop->organisms[count]->Enemies.push_back(initial_pos6[9]);
			pop->organisms[count]->Enemies.push_back(initial_pos6[10]);
			pop->organisms[count]->Enemies.push_back(initial_pos6[11]);

			for (auto &i : enemies6)
			{
				thread_pool.emplace_back(thread(&tank_evaluate, pop->organisms[count], ref(win), num, roomNum));
				num++;
			}
			count++;

			//-----------------
		}
		if (startNextGame7)
		{
			startNextGame7 = false;
			//----------- room7 ----------
			num = 0;
			roomNum++;

			pop->organisms[count]->Users.push_back(initial_pos7[0]);
			pop->organisms[count]->Users.push_back(initial_pos7[1]);
			pop->organisms[count]->Users.push_back(initial_pos7[2]);
			pop->organisms[count]->Users.push_back(initial_pos7[3]);
			pop->organisms[count]->Users.push_back(initial_pos7[4]);
			pop->organisms[count]->Users.push_back(initial_pos7[5]);
			pop->organisms[count]->Enemies.push_back(initial_pos7[6]);
			pop->organisms[count]->Enemies.push_back(initial_pos7[7]);
			pop->organisms[count]->Enemies.push_back(initial_pos7[8]);
			pop->organisms[count]->Enemies.push_back(initial_pos7[9]);
			pop->organisms[count]->Enemies.push_back(initial_pos7[10]);
			pop->organisms[count]->Enemies.push_back(initial_pos7[11]);

			for (auto &i : users7)
			{
				thread_pool.emplace_back(thread(&tank_evaluate, pop->organisms[count], ref(win), num, roomNum));
				num++;
			}
			count++;

			pop->organisms[count]->Users.push_back(initial_pos7[0]);
			pop->organisms[count]->Users.push_back(initial_pos7[1]);
			pop->organisms[count]->Users.push_back(initial_pos7[2]);
			pop->organisms[count]->Users.push_back(initial_pos7[3]);
			pop->organisms[count]->Users.push_back(initial_pos7[4]);
			pop->organisms[count]->Users.push_back(initial_pos7[5]);
			pop->organisms[count]->Enemies.push_back(initial_pos7[6]);
			pop->organisms[count]->Enemies.push_back(initial_pos7[7]);
			pop->organisms[count]->Enemies.push_back(initial_pos7[8]);
			pop->organisms[count]->Enemies.push_back(initial_pos7[9]);
			pop->organisms[count]->Enemies.push_back(initial_pos7[10]);
			pop->organisms[count]->Enemies.push_back(initial_pos7[11]);

			for (auto &i : enemies7)
			{
				thread_pool.emplace_back(thread(&tank_evaluate, pop->organisms[count], ref(win), num, roomNum));
				num++;
			}
			count++;

			//-----------------
		}
		if (startNextGame8)
		{
			startNextGame8 = false;
			//----------- room8 ----------
			num = 0;
			roomNum++;

			pop->organisms[count]->Users.push_back(initial_pos8[0]);
			pop->organisms[count]->Users.push_back(initial_pos8[1]);
			pop->organisms[count]->Users.push_back(initial_pos8[2]);
			pop->organisms[count]->Users.push_back(initial_pos8[3]);
			pop->organisms[count]->Users.push_back(initial_pos8[4]);
			pop->organisms[count]->Users.push_back(initial_pos8[5]);
			pop->organisms[count]->Enemies.push_back(initial_pos8[6]);
			pop->organisms[count]->Enemies.push_back(initial_pos8[7]);
			pop->organisms[count]->Enemies.push_back(initial_pos8[8]);
			pop->organisms[count]->Enemies.push_back(initial_pos8[9]);
			pop->organisms[count]->Enemies.push_back(initial_pos8[10]);
			pop->organisms[count]->Enemies.push_back(initial_pos8[11]);

			for (auto &i : users8)
			{
				thread_pool.emplace_back(thread(&tank_evaluate, pop->organisms[count], ref(win), num, roomNum));
				num++;
			}
			count++;

			pop->organisms[count]->Users.push_back(initial_pos8[0]);
			pop->organisms[count]->Users.push_back(initial_pos8[1]);
			pop->organisms[count]->Users.push_back(initial_pos8[2]);
			pop->organisms[count]->Users.push_back(initial_pos8[3]);
			pop->organisms[count]->Users.push_back(initial_pos8[4]);
			pop->organisms[count]->Users.push_back(initial_pos8[5]);
			pop->organisms[count]->Enemies.push_back(initial_pos8[6]);
			pop->organisms[count]->Enemies.push_back(initial_pos8[7]);
			pop->organisms[count]->Enemies.push_back(initial_pos8[8]);
			pop->organisms[count]->Enemies.push_back(initial_pos8[9]);
			pop->organisms[count]->Enemies.push_back(initial_pos8[10]);
			pop->organisms[count]->Enemies.push_back(initial_pos8[11]);

			for (auto &i : enemies8)
			{
				thread_pool.emplace_back(thread(&tank_evaluate, pop->organisms[count], ref(win), num, roomNum));
				num++;
			}
			count++;
		}

		//담게임 준비완료 신호
		for (auto &i : thread_pool)
		{
			//cout << "here" << endl;
			i.join();
		}

		//---------- Room1 -----------------
		if (!pop->organisms[count - 16]->net->isFailed && !pop->organisms[count - 15]->net->isFailed)
		{
			pop->organisms[count - 16]->fitness = 3000 - ((300 + enemy_hp_sum) * 10) + user_fitness_sum;
			pop->organisms[count - 15]->fitness = 3000 - ((300 + user_hp_sum) * 10) + enemy_fitness_sum;

			cout << "Room1 enemy_hp, user fitness: " << enemy_hp_sum << ", " << user_fitness_sum << endl;
			cout << "Room1 user_hp, enemy fitness: " << user_hp_sum << ", " << enemy_fitness_sum << endl;

			if (fitness_rank.size() < 10)
			{
				fitness_rank.push_back(pop->organisms[count - 16]->fitness);
			}
			if (fitness_rank.size() < 10)
			{
				fitness_rank.push_back(pop->organisms[count - 15]->fitness);
			}
			else
			{
				sort(fitness_rank.rbegin(), fitness_rank.rend());
				if (fitness_rank[9] < pop->organisms[count - 16]->fitness)
				{
					fitness_rank[9] = pop->organisms[count - 16]->fitness;
				}
				if (fitness_rank[9] < pop->organisms[count - 15]->fitness)
				{
					fitness_rank[9] = pop->organisms[count - 15]->fitness;
				}
			}
		}
		else
		{
			pop->organisms[count - 16]->fitness = 0;
			pop->organisms[count - 15]->fitness = 0;
		}

		//---------- Room2 -----------------
		if (!pop->organisms[count - 14]->net->isFailed && !pop->organisms[count - 13]->net->isFailed)
		{
			pop->organisms[count - 14]->fitness = 3000 - ((300 + enemy_hp_sum2) * 10) + user_fitness_sum2;
			pop->organisms[count - 13]->fitness = 3000 - ((300 + user_hp_sum2) * 10) + enemy_fitness_sum2;

			cout << "Room2 enemy_hp, user fitness: " << enemy_hp_sum2 << ", " << user_fitness_sum2 << endl;
			cout << "Room2 user_hp, enemy fitness: " << user_hp_sum2 << ", " << enemy_fitness_sum2 << endl;

			if (fitness_rank.size() < 10)
			{
				fitness_rank.push_back(pop->organisms[count - 14]->fitness);
			}
			if (fitness_rank.size() < 10)
			{
				fitness_rank.push_back(pop->organisms[count - 13]->fitness);
			}
			else
			{
				sort(fitness_rank.rbegin(), fitness_rank.rend());
				if (fitness_rank[9] < pop->organisms[count - 14]->fitness)
				{
					fitness_rank[9] = pop->organisms[count - 14]->fitness;
				}
				if (fitness_rank[9] < pop->organisms[count - 13]->fitness)
				{
					fitness_rank[9] = pop->organisms[count - 13]->fitness;
				}
			}
		}
		else
		{
			pop->organisms[count - 14]->fitness = 0;
			pop->organisms[count - 13]->fitness = 0;
		}

		//---------- Room3 -----------------
		if (!pop->organisms[count - 12]->net->isFailed && !pop->organisms[count - 11]->net->isFailed)
		{
			pop->organisms[count - 12]->fitness = 3000 - ((300 + enemy_hp_sum3) * 10) + user_fitness_sum3;
			pop->organisms[count - 11]->fitness = 3000 - ((300 + user_hp_sum3) * 10) + enemy_fitness_sum3;

			cout << "Room3 enemy_hp, user fitness: " << enemy_hp_sum3 << ", " << user_fitness_sum3 << endl;
			cout << "Room3 user_hp, enemy fitness: " << user_hp_sum3 << ", " << enemy_fitness_sum3 << endl;

			if (fitness_rank.size() < 10)
			{
				fitness_rank.push_back(pop->organisms[count - 12]->fitness);
			}
			if (fitness_rank.size() < 10)
			{
				fitness_rank.push_back(pop->organisms[count - 11]->fitness);
			}
			else
			{
				sort(fitness_rank.rbegin(), fitness_rank.rend());
				if (fitness_rank[9] < pop->organisms[count - 12]->fitness)
				{
					fitness_rank[9] = pop->organisms[count - 12]->fitness;
				}
				if (fitness_rank[9] < pop->organisms[count - 11]->fitness)
				{
					fitness_rank[9] = pop->organisms[count - 11]->fitness;
				}
			}
		}
		else
		{
			pop->organisms[count - 12]->fitness = 0;
			pop->organisms[count - 11]->fitness = 0;
		}

		//---------- Room4 -----------------
		if (!pop->organisms[count - 10]->net->isFailed && !pop->organisms[count - 9]->net->isFailed)
		{
			pop->organisms[count - 10]->fitness = 3000 - ((300 + enemy_hp_sum4) * 10) + user_fitness_sum4;
			pop->organisms[count - 9]->fitness = 3000 - ((300 + user_hp_sum4) * 10) + enemy_fitness_sum4;

			cout << "Room4 enemy_hp, user fitness: " << enemy_hp_sum4 << ", " << user_fitness_sum4 << endl;
			cout << "Room4 user_hp, enemy fitness: " << user_hp_sum4 << ", " << enemy_fitness_sum4 << endl;

			if (fitness_rank.size() < 10)
			{
				fitness_rank.push_back(pop->organisms[count - 10]->fitness);
			}
			if (fitness_rank.size() < 10)
			{
				fitness_rank.push_back(pop->organisms[count - 9]->fitness);
			}
			else
			{
				sort(fitness_rank.rbegin(), fitness_rank.rend());
				if (fitness_rank[9] < pop->organisms[count - 10]->fitness)
				{
					fitness_rank[9] = pop->organisms[count - 10]->fitness;
				}
				if (fitness_rank[9] < pop->organisms[count - 9]->fitness)
				{
					fitness_rank[9] = pop->organisms[count - 9]->fitness;
				}
			}
		}
		else
		{
			pop->organisms[count - 10]->fitness = 0;
			pop->organisms[count - 9]->fitness = 0;
		}

		//---------- Room5 -----------------
		if (!pop->organisms[count - 8]->net->isFailed && !pop->organisms[count - 7]->net->isFailed)
		{
			pop->organisms[count - 8]->fitness = 3000 - ((300 + enemy_hp_sum5) * 10) + user_fitness_sum5;
			pop->organisms[count - 7]->fitness = 3000 - ((300 + user_hp_sum5) * 10) + enemy_fitness_sum5;

			cout << "Room5 enemy_hp, user fitness: " << enemy_hp_sum5 << ", " << user_fitness_sum5 << endl;
			cout << "Room5 user_hp, enemy fitness: " << user_hp_sum5 << ", " << enemy_fitness_sum5 << endl;

			if (fitness_rank.size() < 10)
			{
				fitness_rank.push_back(pop->organisms[count - 8]->fitness);
			}
			if (fitness_rank.size() < 10)
			{
				fitness_rank.push_back(pop->organisms[count - 7]->fitness);
			}
			else
			{
				sort(fitness_rank.rbegin(), fitness_rank.rend());
				if (fitness_rank[9] < pop->organisms[count - 8]->fitness)
				{
					fitness_rank[9] = pop->organisms[count - 8]->fitness;
				}
				if (fitness_rank[9] < pop->organisms[count - 7]->fitness)
				{
					fitness_rank[9] = pop->organisms[count - 7]->fitness;
				}
			}
		}
		else
		{
			pop->organisms[count - 8]->fitness = 0;
			pop->organisms[count - 7]->fitness = 0;
		}

		//---------- Room6 -----------------
		if (!pop->organisms[count - 6]->net->isFailed && !pop->organisms[count - 5]->net->isFailed)
		{
			pop->organisms[count - 6]->fitness = 3000 - ((300 + enemy_hp_sum6) * 10) + user_fitness_sum6;
			pop->organisms[count - 5]->fitness = 3000 - ((300 + user_hp_sum6) * 10) + enemy_fitness_sum6;

			cout << "Room6 enemy_hp, user fitness: " << enemy_hp_sum6 << ", " << user_fitness_sum6 << endl;
			cout << "Room6 user_hp, enemy fitness: " << user_hp_sum6 << ", " << enemy_fitness_sum6 << endl;

			if (fitness_rank.size() < 10)
			{
				fitness_rank.push_back(pop->organisms[count - 6]->fitness);
			}
			if (fitness_rank.size() < 10)
			{
				fitness_rank.push_back(pop->organisms[count - 5]->fitness);
			}
			else
			{
				sort(fitness_rank.rbegin(), fitness_rank.rend());
				if (fitness_rank[9] < pop->organisms[count - 6]->fitness)
				{
					fitness_rank[9] = pop->organisms[count - 6]->fitness;
				}
				if (fitness_rank[9] < pop->organisms[count - 5]->fitness)
				{
					fitness_rank[9] = pop->organisms[count - 5]->fitness;
				}
			}
		}
		else
		{
			pop->organisms[count - 6]->fitness = 0;
			pop->organisms[count - 5]->fitness = 0;
		}

		//---------- Room7 -----------------
		if (!pop->organisms[count - 4]->net->isFailed && !pop->organisms[count - 3]->net->isFailed)
		{
			pop->organisms[count - 4]->fitness = 3000 - ((300 + enemy_hp_sum7) * 10) + user_fitness_sum7;
			pop->organisms[count - 3]->fitness = 3000 - ((300 + user_hp_sum7) * 10) + enemy_fitness_sum7;

			cout << "Room7 enemy_hp, user fitness: " << enemy_hp_sum7 << ", " << user_fitness_sum7 << endl;
			cout << "Room7 user_hp, enemy fitness: " << user_hp_sum7 << ", " << enemy_fitness_sum7 << endl;

			if (fitness_rank.size() < 10)
			{
				fitness_rank.push_back(pop->organisms[count - 4]->fitness);
			}
			if (fitness_rank.size() < 10)
			{
				fitness_rank.push_back(pop->organisms[count - 3]->fitness);
			}
			else
			{
				sort(fitness_rank.rbegin(), fitness_rank.rend());
				if (fitness_rank[9] < pop->organisms[count - 4]->fitness)
				{
					fitness_rank[9] = pop->organisms[count - 4]->fitness;
				}
				if (fitness_rank[9] < pop->organisms[count - 3]->fitness)
				{
					fitness_rank[9] = pop->organisms[count - 3]->fitness;
				}
			}
		}
		else
		{
			pop->organisms[count - 4]->fitness = 0;
			pop->organisms[count - 3]->fitness = 0;
		}

		//---------- Room8 -----------------
		if (!pop->organisms[count - 2]->net->isFailed && !pop->organisms[count - 1]->net->isFailed)
		{
			pop->organisms[count - 2]->fitness = 3000 - ((300 + enemy_hp_sum8) * 10) + user_fitness_sum8;
			pop->organisms[count - 1]->fitness = 3000 - ((300 + user_hp_sum8) * 10) + enemy_fitness_sum8;

			cout << "Room8 enemy_hp, user fitness: " << enemy_hp_sum8 << ", " << user_fitness_sum8 << endl;
			cout << "Room8 user_hp, enemy fitness: " << user_hp_sum8 << ", " << enemy_fitness_sum8 << endl;

			if (fitness_rank.size() < 10)
			{
				fitness_rank.push_back(pop->organisms[count - 2]->fitness);
			}
			if (fitness_rank.size() < 10)
			{
				fitness_rank.push_back(pop->organisms[count - 1]->fitness);
			}
			else
			{
				sort(fitness_rank.rbegin(), fitness_rank.rend());
				if (fitness_rank[9] < pop->organisms[count - 2]->fitness)
				{
					fitness_rank[9] = pop->organisms[count - 2]->fitness;
				}
				if (fitness_rank[9] < pop->organisms[count - 1]->fitness)
				{
					fitness_rank[9] = pop->organisms[count - 1]->fitness;
				}
			}
		}
		else
		{
			pop->organisms[count - 2]->fitness = 0;
			pop->organisms[count - 1]->fitness = 0;
		}


		//cout << "join!" << endl;
		//if (user_hp_sum == 0)
		//	enemy_fitness_sum += 200; //적이 이겼을 경우 fitness 200넣어줌
		//if (enemy_hp_sum == 0)
		//	user_fitness_sum += 200; //user가 이겼을 경우 fitness 200넣어줌

		//각 게임에 대한 fitness를 측정하고 여기서 더함. fitness측정은 어떻게?
		//우선..게임 끝났을 때의 상태를 저장하고 해당 정보를 바탕으로
		//탱크 몇 마리가 데미지를 얼만큼 데미지를 입었는지..

		user_fitness_sum = 0;
		enemy_fitness_sum = 0;
		user_hp_sum = 0;
		enemy_hp_sum = 0;

		user_fitness_sum2 = 0;
		enemy_fitness_sum2 = 0;
		user_hp_sum2 = 0;
		enemy_hp_sum2 = 0;

		user_fitness_sum3 = 0;
		enemy_fitness_sum3 = 0;
		user_hp_sum3 = 0;
		enemy_hp_sum3 = 0;

		user_fitness_sum4 = 0;
		enemy_fitness_sum4 = 0;
		user_hp_sum4 = 0;
		enemy_hp_sum4 = 0;

		user_fitness_sum5 = 0;
		enemy_fitness_sum5 = 0;
		user_hp_sum5 = 0;
		enemy_hp_sum5 = 0;

		user_fitness_sum6 = 0;
		enemy_fitness_sum6 = 0;
		user_hp_sum6 = 0;
		enemy_hp_sum6 = 0;

		user_fitness_sum7 = 0;
		enemy_fitness_sum7 = 0;
		user_hp_sum7 = 0;
		enemy_hp_sum7 = 0;

		user_fitness_sum8 = 0;
		enemy_fitness_sum8 = 0;
		user_hp_sum8 = 0;
		enemy_hp_sum8 = 0;

		thread_pool.clear();
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

	sort(fitness_rank.rbegin(), fitness_rank.rend());
	for (auto &i : pop->organisms)
	{
		if (i->fitness >= fitness_rank[9])
			i->print_to_file(filename);
	}

	fitness_rank.clear();

	//Only print to file every print_every generations
	//if (win || ((generation % (NEAT::print_every)) == 0))
	//	pop->print_to_file_by_species(filename);

	//여기서 여러파일 출력 담당!

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

int try_tank(Network * net, int max_steps, int thresh, int num, int roomNum)
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
	int fitness = 0;

	int random = 0;

	switch (roomNum)
	{
	case 0:
		while (GameOver)
		{

		}
		break;
	case 1:
		while (GameOver2)
		{

		}
		break;
	case 2:
		while (GameOver3)
		{

		}
		break;
	case 3:
		while (GameOver4)
		{

		}
		break;
	case 4:
		while (GameOver5)
		{

		}
		break;
	case 5:
		while (GameOver6)
		{

		}
		break;
	case 6:
		while (GameOver7)
		{

		}
		break;
	case 7:
		while (GameOver8)
		{

		}
		break;
	default:
		break;
	}

	vector<User*> userArr;
	vector<Enemy*> enemyArr;

	switch (roomNum)
	{
	case 0:
	{
		userArr = users;
		enemyArr = enemies;
		break;
	}
	case 1:
	{
		userArr = users2;
		enemyArr = enemies2;
		break;
	}
	case 2:
	{
		userArr = users3;
		enemyArr = enemies3;
		break;
	}
	case 3:
	{
		userArr = users4;
		enemyArr = enemies4;
		break;
	}
	case 4:
	{
		userArr = users5;
		enemyArr = enemies5;
		break;
	}
	case 5:
	{
		userArr = users6;
		enemyArr = enemies6;
		break;
	}
	case 6:
	{
		userArr = users7;
		enemyArr = enemies7;
		break;
	}
	case 7:
	{
		userArr = users8;
		enemyArr = enemies8;
		break;
	}
	default:
		break;
	}

	//mutex_2.lock();
	for (auto &i : userArr)
	{
		i->gun->roomNum = roomNum;
	}
	for (auto &i : enemyArr)
	{
		i->gun->roomNum = roomNum;
	}
	//mutex_2.unlock();

	mutex_2.lock();
	if (num < 3)
	{
		userArr[num]->isDie = false;
		//userArr[num]->hp = 100;
		userArr[num]->angle = 90.0;
		userArr[num]->c_angle = 0.0;
	}
	else
	{
		enemyArr[num - 3]->isDie = false;
		//enemyArr[num - 3]->hp = 100;
		enemyArr[num - 3]->angle = 270.0;
		enemyArr[num - 3]->c_angle = 0.0;
	}
	mutex_2.unlock();
	
	atomic<bool> &isOver = findRoomGameOver(roomNum);
	while (!isOver) //다음으로 넘어갈 조건..->게임오버
	{
		//Measure만 넣으면 됨!!!
		mutex_2.lock();
		double out_Left;
		double out_Right;
		double out_isShoot;
		double out_angle;
		double in[12];

		//상대좌표 구하고 역행렬회전하면됨!
		if (num < 3)
		{
			//User(아래쪽)
			in[0] = 0.1;
			//in[1] = users[num]->x;
			//in[2] = users[num]->y;
			if (num == 0)
			{
				//in[3] = users[num + 1]->x;
				//in[4] = users[num + 1]->y;
				//in[5] = users[num + 2]->x;
				//in[6] = users[num + 2]->y;

				float diffX = userArr[num + 1]->x - userArr[num]->x;
				float diffY = userArr[num + 1]->y - userArr[num]->y;

				in[1] = cos(userArr[num]->c_angle / 180 * 3.14159) * diffX + sin(userArr[num]->c_angle / 180 * 3.14159) * diffY;
				in[2] = -sin(userArr[num]->c_angle / 180 * 3.14159) * diffX + cos(userArr[num]->c_angle / 180 * 3.14159) * diffY;

				diffX = userArr[num + 2]->x - userArr[num]->x;
				diffY = userArr[num + 2]->y - userArr[num]->y;

				in[3] = cos(userArr[num]->c_angle / 180 * 3.14159) * diffX + sin(userArr[num]->c_angle / 180 * 3.14159) * diffY;
				in[4] = -sin(userArr[num]->c_angle / 180 * 3.14159) * diffX + cos(userArr[num]->c_angle / 180 * 3.14159) * diffY;
			}
			else if (num == 1)
			{
				//in[3] = users[num - 1]->x;
				//in[4] = users[num - 1]->y;
				//in[5] = users[num + 1]->x;
				//in[6] = users[num + 1]->y;

				float diffX = userArr[num - 1]->x - userArr[num]->x;
				float diffY = userArr[num - 1]->y - userArr[num]->y;

				in[1] = cos(userArr[num]->c_angle / 180 * 3.14159) * diffX + sin(userArr[num]->c_angle / 180 * 3.14159) * diffY;
				in[2] = -sin(userArr[num]->c_angle / 180 * 3.14159) * diffX + cos(userArr[num]->c_angle / 180 * 3.14159) * diffY;

				diffX = userArr[num + 1]->x - userArr[num]->x;
				diffY = userArr[num + 1]->y - userArr[num]->y;

				in[3] = cos(userArr[num]->c_angle / 180 * 3.14159) * diffX + sin(userArr[num]->c_angle / 180 * 3.14159) * diffY;
				in[4] = -sin(userArr[num]->c_angle / 180 * 3.14159) * diffX + cos(userArr[num]->c_angle / 180 * 3.14159) * diffY;
			}
			else
			{
				//in[3] = users[num - 2]->x;
				//in[4] = users[num - 2]->y;
				//in[5] = users[num - 1]->x;
				//in[6] = users[num - 1]->y;

				float diffX = userArr[num - 2]->x - userArr[num]->x;
				float diffY = userArr[num - 2]->y - userArr[num]->y;

				in[1] = cos(userArr[num]->c_angle / 180 * 3.14159) * diffX + sin(userArr[num]->c_angle / 180 * 3.14159) * diffY;
				in[2] = -sin(userArr[num]->c_angle / 180 * 3.14159) * diffX + cos(userArr[num]->c_angle / 180 * 3.14159) * diffY;

				diffX = userArr[num - 1]->x - userArr[num]->x;
				diffY = userArr[num - 1]->y - userArr[num]->y;

				in[3] = cos(userArr[num]->c_angle / 180 * 3.14159) * diffX + sin(userArr[num]->c_angle / 180 * 3.14159) * diffY;
				in[4] = -sin(userArr[num]->c_angle / 180 * 3.14159) * diffX + cos(userArr[num]->c_angle / 180 * 3.14159) * diffY;
			}

			float diffX = enemyArr[0]->x - userArr[num]->x;
			float diffY = enemyArr[0]->y - userArr[num]->y;

			in[5] = cos(userArr[num]->c_angle / 180 * 3.14159) * diffX + sin(userArr[num]->c_angle / 180 * 3.14159) * diffY;
			in[6] = -sin(userArr[num]->c_angle / 180 * 3.14159) * diffX + cos(userArr[num]->c_angle / 180 * 3.14159) * diffY;

			diffX = enemyArr[1]->x - userArr[num]->x;
			diffY = enemyArr[1]->y - userArr[num]->y;

			in[7] = cos(userArr[num]->c_angle / 180 * 3.14159) * diffX + sin(userArr[num]->c_angle / 180 * 3.14159) * diffY;
			in[8] = -sin(userArr[num]->c_angle / 180 * 3.14159) * diffX + cos(userArr[num]->c_angle / 180 * 3.14159) * diffY;

			diffX = enemyArr[2]->x - userArr[num]->x;
			diffY = enemyArr[2]->y - userArr[num]->y;

			in[9] = cos(userArr[num]->c_angle / 180 * 3.14159) * diffX + sin(userArr[num]->c_angle / 180 * 3.14159) * diffY;
			in[10] = -sin(userArr[num]->c_angle / 180 * 3.14159) * diffX + cos(userArr[num]->c_angle / 180 * 3.14159) * diffY;
			//in[11] = ((userArr[num]->angle - 90 + (userArr[num]->c_angle * 180)) / 3.14159);
			in[11] = (userArr[num]->angle - 90) * 3.14159 / 180;
		}
		else
		{
			in[0] = 0.1;
			//in[1] = enemies[num - 3]->x;
			//in[2] = enemies[num - 3]->y;
			if (num == 3)
			{
				//in[3] = enemies[num - 2]->x;
				//in[4] = enemies[num - 2]->y;
				//in[5] = enemies[num - 1]->x;
				//in[6] = enemies[num - 1]->y;

				float diffX = enemyArr[num - 2]->x - enemyArr[num - 3]->x;
				float diffY = enemyArr[num - 2]->y - enemyArr[num - 3]->y;

				in[1] = cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
				in[2] = -sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;

				diffX = enemyArr[num - 1]->x - enemyArr[num - 3]->x;
				diffY = enemyArr[num - 1]->y - enemyArr[num - 3]->y;

				in[3] = cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
				in[4] = -sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
			}
			else if (num == 4)
			{
				//in[3] = enemies[num - 4]->x;
				//in[4] = enemies[num - 4]->y;
				//in[5] = enemies[num - 2]->x;
				//in[6] = enemies[num - 2]->y;
				float diffX = enemyArr[num - 4]->x - enemyArr[num - 3]->x;
				float diffY = enemyArr[num - 4]->y - enemyArr[num - 3]->y;

				in[1] = cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
				in[2] = -sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;

				diffX = enemyArr[num - 2]->x - enemyArr[num - 3]->x;
				diffY = enemyArr[num - 2]->y - enemyArr[num - 3]->y;

				in[3] = cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
				in[4] = -sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
			}
			else
			{
				//in[3] = enemies[num - 5]->x;
				//in[4] = enemies[num - 5]->y;
				//in[5] = enemies[num - 4]->x;
				//in[6] = enemies[num - 4]->y;

				float diffX = enemyArr[num - 5]->x - enemyArr[num - 3]->x;
				float diffY = enemyArr[num - 5]->y - enemyArr[num - 3]->y;

				in[1] = cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
				in[2] = -sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;

				diffX = enemyArr[num - 4]->x - enemyArr[num - 3]->x;
				diffY = enemyArr[num - 4]->y - enemyArr[num - 3]->y;

				in[3] = cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
				in[4] = -sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
			}
			/*in[7] = users[0]->x;
			in[8] = users[0]->y;
			in[9] = users[1]->x;
			in[10] = users[1]->y;
			in[11] = users[2]->x;
			in[12] = users[2]->y;
			in[13] = (enemies[num - 3]->angle - 90 + enemies[num - 3]->c_angle * 180 / 3.14159);*/
			float diffX = userArr[0]->x - enemyArr[num - 3]->x;
			float diffY = userArr[0]->y - enemyArr[num - 3]->y;

			in[5] = cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
			in[6] = -sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;

			diffX = userArr[1]->x - enemyArr[num - 3]->x;
			diffY = userArr[1]->y - enemyArr[num - 3]->y;

			in[7] = cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
			in[8] = -sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;

			diffX = userArr[2]->x - enemyArr[num - 3]->x;
			diffY = userArr[2]->y - enemyArr[num - 3]->y;

			in[9] = cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
			in[10] = -sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
			//in[11] = ((enemyArr[num - 3]->angle - 90 + (enemyArr[num - 3]->c_angle * 180)) / 3.14159);
			in[11] = (enemyArr[num - 3]->angle - 90) / 180 * 3.14159;
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
		//out_angle = (*out_iter)->activation;
		out_angle = net->outputs[3]->activation;

		if (out_angle < 0.1)
			out_angle = 0.1;

		if (out_Left > 1)
		{
			out_Left = 0;
			cout << "overflow!!" << endl;
		}
		if (out_Right > 1)
		{
			out_Right = 0;
			cout << "overflow2!!" << endl;
		}
		out_angle -= 0.5;

		//if(out_Left)
		//out_Left = out_Left / 50;
		//out_Right = out_Right / 50;
		// 여기서 fitness penalty를 주자

		if (num < 3)
		{
			if (userArr[num]->sameDirCount == 0)
			{
				if (out_Left > out_Right)
					userArr[num]->previousDir = false;
				else
					userArr[num]->previousDir = true;

				userArr[num]->sameDirCount++;
			}
			else
			{
				if (!userArr[num]->previousDir)
				{
					if (out_Left > out_Right)
						userArr[num]->sameDirCount++;
					else
						userArr[num]->sameDirCount = 0;
				}
				else
				{
					if (out_Left < out_Right)
						userArr[num]->sameDirCount++;
					else
						userArr[num]->sameDirCount = 0;
				}
			}
		}
		else
		{
			if (enemyArr[num - 3]->sameDirCount == 0)
			{
				if (out_Left > out_Right)
					enemyArr[num - 3]->previousDir = false;
				else
					enemyArr[num - 3]->previousDir = true;

				enemyArr[num - 3]->sameDirCount++;
			}
			else
			{
				if (!enemyArr[num - 3]->previousDir)
				{
					if (out_Left > out_Right)
						enemyArr[num - 3]->sameDirCount++;
					else
						enemyArr[num - 3]->sameDirCount = 0;
				}
				else
				{
					if (out_Left < out_Right)
						enemyArr[num - 3]->sameDirCount++;
					else
						enemyArr[num - 3]->sameDirCount = 0;
				}
			}
		}


		double copyShoot = out_isShoot;
		out_isShoot = out_isShoot - copyShoot * 0.3 + 0.3;
		
		if (abs(out_Left - out_Right) > 0.5)
			fitness--;

		if (num < 3)
		{
			if (roomNum == 7)
			{
				cout << "작동중" << endl;
			}
			userArr[num]->MoveUser(out_Left / 10, out_Right / 10);
			userArr[num]->RotateCannon(out_angle);
			if (out_isShoot > 0)
				userArr[num]->gun->RateOfShoot = out_isShoot;
				//users[num]->gun->Shoot();
			if (userArr[num]->sameDirCount > 50)
				fitness--;
		}
		else
		{
			enemyArr[num - 3]->MoveUser(out_Left / 10, out_Right / 10);
			enemyArr[num - 3]->RotateCannon(out_angle);
			if (out_isShoot > 0)
				enemyArr[num - 3]->gun->RateOfShoot = out_isShoot;
				//enemies[num - 3]->gun->Shoot();
			if (enemyArr[num - 3]->sameDirCount > 50)
				fitness--;
		}
		mutex_2.unlock();
		Sleep(50);
		random = randbtn(1, 10000);
	}

	switch (roomNum)
	{
	case 0:
	{
		if (num < 3)
			user_fitness_sum += fitness;
		else
			enemy_fitness_sum += fitness;
		break;
	}
	case 1:
	{
		if (num < 3)
			user_fitness_sum2 += fitness;
		else
			enemy_fitness_sum2 += fitness;
		break;
	}
	case 2:
	{
		if (num < 3)
			user_fitness_sum3 += fitness;
		else
			enemy_fitness_sum3 += fitness;
		break;
	}
	case 3:
	{
		if (num < 3)
			user_fitness_sum4 += fitness;
		else
			enemy_fitness_sum4 += fitness;
		break;
	}
	case 4:
	{
		if (num < 3)
			user_fitness_sum5 += fitness;
		else
			enemy_fitness_sum5 += fitness;
		break;
	}
	case 5:
	{
		if (num < 3)
			user_fitness_sum6 += fitness;
		else
			enemy_fitness_sum6 += fitness;
		break;
	}
	case 6:
	{
		if (num < 3)
			user_fitness_sum7 += fitness;
		else
			enemy_fitness_sum7 += fitness;
		break;
	}
	case 7:
	{
		if (num < 3)
			user_fitness_sum8 += fitness;
		else
			enemy_fitness_sum8 += fitness;
		break;
	}
	default:
		break;
	}

	return 0;
}

int try_tank_mirror(Network * net, int max_steps, int thresh, int num, int roomNum)
{
	vector<NNode*>::iterator out_iter;
	int fitness = 0;

	int random = 0;

	switch (roomNum)
	{
	case 0:
		while (GameOver)
		{

		}
		break;
	case 1:
		while (GameOver2)
		{

		}
		break;
	case 2:
		while (GameOver3)
		{

		}
		break;
	case 3:
		while (GameOver4)
		{

		}
		break;
	case 4:
		while (GameOver5)
		{

		}
		break;
	case 5:
		while (GameOver6)
		{

		}
		break;
	case 6:
		while (GameOver7)
		{

		}
		break;
	case 7:
		while (GameOver8)
		{

		}
		break;
	default:
		break;
	}

	vector<User*> &userArr = findRoomUserArr(roomNum);
	vector<Enemy*> &enemyArr = findRoomEnemyArr(roomNum);

	/*mutex_2.lock();
	for (auto &i : userArr)
	{
		i->gun->roomNum = roomNum;
	}
	for (auto &i : enemyArr)
	{
		i->gun->roomNum = roomNum;
	}
	mutex_2.unlock();*/

	mutex_2.lock();
	if (num < 3)
	{
		userArr[num]->isDie = false;
		//userArr[num]->hp = 100;
		userArr[num]->angle = 90.0;
		userArr[num]->c_angle = 0.0;
	}
	else
	{
		enemyArr[num - 3]->isDie = false;
		//enemyArr[num - 3]->hp = 100;
		enemyArr[num - 3]->angle = 270.0;
		enemyArr[num - 3]->c_angle = 0.0;
	}
	mutex_2.unlock();

	atomic<bool> &isOver = findRoomGameOver(roomNum);
	while (!isOver) //다음으로 넘어갈 조건..->게임오버
	{
		//Measure만 넣으면 됨!!!
		mutex_2.lock();
		double out_Left;
		double out_Right;
		double out_isShoot;
		double out_angle_right;
		double out_angle_left;
		double in[12];

		//상대좌표 구하고 역행렬회전하면됨!
		if (num < 3)
		{
			//User(아래쪽)
			in[0] = 0.1;
			//in[1] = users[num]->x;
			//in[2] = users[num]->y;
			if (num == 0)
			{
				//in[3] = users[num + 1]->x;
				//in[4] = users[num + 1]->y;
				//in[5] = users[num + 2]->x;
				//in[6] = users[num + 2]->y;

				float diffX = userArr[num + 1]->x - userArr[num]->x;
				float diffY = userArr[num + 1]->y - userArr[num]->y;

				in[1] = cos(userArr[num]->c_angle / 180 * 3.14159) * diffX + sin(userArr[num]->c_angle / 180 * 3.14159) * diffY;
				in[2] = -sin(userArr[num]->c_angle / 180 * 3.14159) * diffX + cos(userArr[num]->c_angle / 180 * 3.14159) * diffY;

				diffX = userArr[num + 2]->x - userArr[num]->x;
				diffY = userArr[num + 2]->y - userArr[num]->y;

				in[3] = cos(userArr[num]->c_angle / 180 * 3.14159) * diffX + sin(userArr[num]->c_angle / 180 * 3.14159) * diffY;
				in[4] = -sin(userArr[num]->c_angle / 180 * 3.14159) * diffX + cos(userArr[num]->c_angle / 180 * 3.14159) * diffY;
			}
			else if (num == 1)
			{
				//in[3] = users[num - 1]->x;
				//in[4] = users[num - 1]->y;
				//in[5] = users[num + 1]->x;
				//in[6] = users[num + 1]->y;

				float diffX = userArr[num - 1]->x - userArr[num]->x;
				float diffY = userArr[num - 1]->y - userArr[num]->y;

				in[1] = cos(userArr[num]->c_angle / 180 * 3.14159) * diffX + sin(userArr[num]->c_angle / 180 * 3.14159) * diffY;
				in[2] = -sin(userArr[num]->c_angle / 180 * 3.14159) * diffX + cos(userArr[num]->c_angle / 180 * 3.14159) * diffY;

				diffX = userArr[num + 1]->x - userArr[num]->x;
				diffY = userArr[num + 1]->y - userArr[num]->y;

				in[3] = cos(userArr[num]->c_angle / 180 * 3.14159) * diffX + sin(userArr[num]->c_angle / 180 * 3.14159) * diffY;
				in[4] = -sin(userArr[num]->c_angle / 180 * 3.14159) * diffX + cos(userArr[num]->c_angle / 180 * 3.14159) * diffY;
			}
			else
			{
				//in[3] = users[num - 2]->x;
				//in[4] = users[num - 2]->y;
				//in[5] = users[num - 1]->x;
				//in[6] = users[num - 1]->y;

				float diffX = userArr[num - 2]->x - userArr[num]->x;
				float diffY = userArr[num - 2]->y - userArr[num]->y;

				in[1] = cos(userArr[num]->c_angle / 180 * 3.14159) * diffX + sin(userArr[num]->c_angle / 180 * 3.14159) * diffY;
				in[2] = -sin(userArr[num]->c_angle / 180 * 3.14159) * diffX + cos(userArr[num]->c_angle / 180 * 3.14159) * diffY;

				diffX = userArr[num - 1]->x - userArr[num]->x;
				diffY = userArr[num - 1]->y - userArr[num]->y;

				in[3] = cos(userArr[num]->c_angle / 180 * 3.14159) * diffX + sin(userArr[num]->c_angle / 180 * 3.14159) * diffY;
				in[4] = -sin(userArr[num]->c_angle / 180 * 3.14159) * diffX + cos(userArr[num]->c_angle / 180 * 3.14159) * diffY;
			}

			float diffX = enemyArr[0]->x - userArr[num]->x;
			float diffY = enemyArr[0]->y - userArr[num]->y;

			in[5] = cos(userArr[num]->c_angle / 180 * 3.14159) * diffX + sin(userArr[num]->c_angle / 180 * 3.14159) * diffY;
			in[6] = -sin(userArr[num]->c_angle / 180 * 3.14159) * diffX + cos(userArr[num]->c_angle / 180 * 3.14159) * diffY;

			diffX = enemyArr[1]->x - userArr[num]->x;
			diffY = enemyArr[1]->y - userArr[num]->y;

			in[7] = cos(userArr[num]->c_angle / 180 * 3.14159) * diffX + sin(userArr[num]->c_angle / 180 * 3.14159) * diffY;
			in[8] = -sin(userArr[num]->c_angle / 180 * 3.14159) * diffX + cos(userArr[num]->c_angle / 180 * 3.14159) * diffY;

			diffX = enemyArr[2]->x - userArr[num]->x;
			diffY = enemyArr[2]->y - userArr[num]->y;

			in[9] = cos(userArr[num]->c_angle / 180 * 3.14159) * diffX + sin(userArr[num]->c_angle / 180 * 3.14159) * diffY;
			in[10] = -sin(userArr[num]->c_angle / 180 * 3.14159) * diffX + cos(userArr[num]->c_angle / 180 * 3.14159) * diffY;
			//in[11] = ((userArr[num]->angle - 90 + (userArr[num]->c_angle * 180)) / 3.14159);
			in[11] = (userArr[num]->angle - 90) * 3.14159 / 180;
		}
		else
		{
			in[0] = 0.1;
			//in[1] = enemies[num - 3]->x;
			//in[2] = enemies[num - 3]->y;
			if (num == 3)
			{
				//in[3] = enemies[num - 2]->x;
				//in[4] = enemies[num - 2]->y;
				//in[5] = enemies[num - 1]->x;
				//in[6] = enemies[num - 1]->y;

				float diffX = enemyArr[num - 2]->x - enemyArr[num - 3]->x;
				float diffY = enemyArr[num - 2]->y - enemyArr[num - 3]->y;

				in[1] = cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
				in[2] = -sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;

				diffX = enemyArr[num - 1]->x - enemyArr[num - 3]->x;
				diffY = enemyArr[num - 1]->y - enemyArr[num - 3]->y;

				in[3] = cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
				in[4] = -sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
			}
			else if (num == 4)
			{
				//in[3] = enemies[num - 4]->x;
				//in[4] = enemies[num - 4]->y;
				//in[5] = enemies[num - 2]->x;
				//in[6] = enemies[num - 2]->y;
				float diffX = enemyArr[num - 4]->x - enemyArr[num - 3]->x;
				float diffY = enemyArr[num - 4]->y - enemyArr[num - 3]->y;

				in[1] = cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
				in[2] = -sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;

				diffX = enemyArr[num - 2]->x - enemyArr[num - 3]->x;
				diffY = enemyArr[num - 2]->y - enemyArr[num - 3]->y;

				in[3] = cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
				in[4] = -sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
			}
			else
			{
				//in[3] = enemies[num - 5]->x;
				//in[4] = enemies[num - 5]->y;
				//in[5] = enemies[num - 4]->x;
				//in[6] = enemies[num - 4]->y;

				float diffX = enemyArr[num - 5]->x - enemyArr[num - 3]->x;
				float diffY = enemyArr[num - 5]->y - enemyArr[num - 3]->y;

				in[1] = cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
				in[2] = -sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;

				diffX = enemyArr[num - 4]->x - enemyArr[num - 3]->x;
				diffY = enemyArr[num - 4]->y - enemyArr[num - 3]->y;

				in[3] = cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
				in[4] = -sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
			}
			/*in[7] = users[0]->x;
			in[8] = users[0]->y;
			in[9] = users[1]->x;
			in[10] = users[1]->y;
			in[11] = users[2]->x;
			in[12] = users[2]->y;
			in[13] = (enemies[num - 3]->angle - 90 + enemies[num - 3]->c_angle * 180 / 3.14159);*/
			float diffX = userArr[0]->x - enemyArr[num - 3]->x;
			float diffY = userArr[0]->y - enemyArr[num - 3]->y;

			in[5] = cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
			in[6] = -sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;

			diffX = userArr[1]->x - enemyArr[num - 3]->x;
			diffY = userArr[1]->y - enemyArr[num - 3]->y;

			in[7] = cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
			in[8] = -sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;

			diffX = userArr[2]->x - enemyArr[num - 3]->x;
			diffY = userArr[2]->y - enemyArr[num - 3]->y;

			in[9] = cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
			in[10] = -sin(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffX + cos(enemyArr[num - 3]->c_angle / 180 * 3.14159) * diffY;
			//in[11] = ((enemyArr[num - 3]->angle - 90 + (enemyArr[num - 3]->c_angle * 180)) / 3.14159);
			in[11] = (enemyArr[num - 3]->angle - 90) / 180 * 3.14159;
		}

		net->load_sensors(in);

		if (!(net->activate()))
			return 1;

		out_iter = net->outputs.begin();
		out_Right = (*out_iter)->activation;
		++out_iter;
		out_angle_right = (*out_iter)->activation;
		++out_iter;
		//out_angle = (*out_iter)->activation;
		out_isShoot = net->outputs[2]->activation;


		//input값 부호 반대로
		in[1] = -in[1];
		in[3] = -in[3];
		in[5] = -in[5];
		in[7] = -in[7];
		in[9] = -in[9];
		in[11] = -in[11];
		net->load_sensors(in);

		if (!(net->activate()))
			return 1;

		out_iter = net->outputs.begin();
		out_Left = (*out_iter)->activation;
		++out_iter;
		out_angle_left = (*out_iter)->activation;
		++out_iter;
		//out_angle = (*out_iter)->activation;
		out_isShoot += net->outputs[2]->activation;

		if (num < 3)
		{
			if (userArr[num]->sameDirCount == 0)
			{
				if (out_Left > out_Right)
					userArr[num]->previousDir = false;
				else
					userArr[num]->previousDir = true;

				userArr[num]->sameDirCount++;
			}
			else
			{
				if (!userArr[num]->previousDir)
				{
					if (out_Left > out_Right)
						userArr[num]->sameDirCount++;
					else
						userArr[num]->sameDirCount = 0;
				}
				else
				{
					if (out_Left < out_Right)
						userArr[num]->sameDirCount++;
					else
						userArr[num]->sameDirCount = 0;
				}
			}
		}
		else
		{
			if (enemyArr[num - 3]->sameDirCount == 0)
			{
				if (out_Left > out_Right)
					enemyArr[num - 3]->previousDir = false;
				else
					enemyArr[num - 3]->previousDir = true;

				enemyArr[num - 3]->sameDirCount++;
			}
			else
			{
				if (!enemyArr[num - 3]->previousDir)
				{
					if (out_Left > out_Right)
						enemyArr[num - 3]->sameDirCount++;
					else
						enemyArr[num - 3]->sameDirCount = 0;
				}
				else
				{
					if (out_Left < out_Right)
						enemyArr[num - 3]->sameDirCount++;
					else
						enemyArr[num - 3]->sameDirCount = 0;
				}
			}
		}

		double out_angle = out_angle_right - out_angle_left;

		double copyShoot = out_isShoot;
		out_isShoot = out_isShoot - copyShoot * 0.3 + 0.3;

		if (abs(out_Left - out_Right) > 0.5)
			fitness--;

		if (num < 3)
		{
			userArr[num]->MoveUser(out_Right * 30, out_Left * 30);
			userArr[num]->RotateCannon(out_angle);
			if (out_isShoot > 0)
				userArr[num]->gun->RateOfShoot = out_isShoot / 2;
			//users[num]->gun->Shoot();
			//if (userArr[num]->sameDirCount > 50)
			//	fitness--;
		}
		else
		{
			enemyArr[num - 3]->MoveUser(out_Right * 30, out_Left * 30);
			enemyArr[num - 3]->RotateCannon(out_angle);
			if (out_isShoot > 0)
				enemyArr[num - 3]->gun->RateOfShoot = out_isShoot / 2;
			//enemies[num - 3]->gun->Shoot();
			//if (enemyArr[num - 3]->sameDirCount > 50)
			//	fitness--;
		}

		mutex_2.unlock();
		Sleep(50);
		random = randbtn(1, 10000);
	}

	/*switch (roomNum)
	{
	case 0:
	{
		if (num < 3)
			user_fitness_sum += fitness;
		else
			enemy_fitness_sum += fitness;
		break;
	}
	case 1:
	{
		if (num < 3)
			user_fitness_sum2 += fitness;
		else
			enemy_fitness_sum2 += fitness;
		break;
	}
	case 2:
	{
		if (num < 3)
			user_fitness_sum3 += fitness;
		else
			enemy_fitness_sum3 += fitness;
		break;
	}
	case 3:
	{
		if (num < 3)
			user_fitness_sum4 += fitness;
		else
			enemy_fitness_sum4 += fitness;
		break;
	}
	case 4:
	{
		if (num < 3)
			user_fitness_sum5 += fitness;
		else
			enemy_fitness_sum5 += fitness;
		break;
	}
	case 5:
	{
		if (num < 3)
			user_fitness_sum6 += fitness;
		else
			enemy_fitness_sum6 += fitness;
		break;
	}
	case 6:
	{
		if (num < 3)
			user_fitness_sum7 += fitness;
		else
			enemy_fitness_sum7 += fitness;
		break;
	}
	case 7:
	{
		if (num < 3)
			user_fitness_sum8 += fitness;
		else
			enemy_fitness_sum8 += fitness;
		break;
	}
	default:
		break;
	}*/

	return 0;
}

atomic<bool>& findRoomGameOver(int roomNum)
{
	switch (roomNum)
	{
	case 0:
	{
		return GameOver;
		break;
	}
	case 1:
	{
		return GameOver2;
		break;
	}
	case 2:
	{
		return GameOver3;
		break;
	}
	case 3:
	{
		return GameOver4;
		break;
	}
	case 4:
	{
		return GameOver5;
		break;
	}
	case 5:
	{
		return GameOver6;
		break;
	}
	case 6:
	{
		return GameOver7;
		break;
	}
	case 7:
	{
		return GameOver8;
		break;
	}
	default:
		return GameOver;
		break;
	}
}

vector<User*>& findRoomUserArr(int roomNum)
{
	switch (roomNum)
	{
	case 0:
	{
		return users;
		break;
	}
	case 1:
	{
		return users2;
		break;
	}
	case 2:
	{
		return users3;
		break;
	}
	case 3:
	{
		return users4;
		break;
	}
	case 4:
	{
		return users5;
		break;
	}
	case 5:
	{
		return users6;
		break;
	}
	case 6:
	{
		return users7;
		break;
	}
	case 7:
	{
		return users8;
		break;
	}
	default:
		return users;
		break;
	}
}

vector<Enemy*>& findRoomEnemyArr(int roomNum)
{
	switch (roomNum)
	{
	case 0:
	{
		return enemies;
		break;
	}
	case 1:
	{
		return enemies2;
		break;
	}
	case 2:
	{
		return enemies3;
		break;
	}
	case 3:
	{
		return enemies4;
		break;
	}
	case 4:
	{
		return enemies5;
		break;
	}
	case 5:
	{
		return enemies6;
		break;
	}
	case 6:
	{
		return enemies7;
		break;
	}
	case 7:
	{
		return enemies8;
		break;
	}
	default:
		return enemies;
		break;
	}
}

