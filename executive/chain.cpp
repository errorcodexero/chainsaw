#include "chain.h"
#include "teleop.h"

using namespace std;

Chain::Chain(vector<Step> s, Executive n):current_step(0),steps(s),next(n){
	if(steps.size()==0) steps.push_back(Step{Wait(0)});
}
Chain::Chain(Step s, Executive n):Chain(vector<Step>{s},n){}

/*Chain::Chain(Chain const& a):step(a.step){
	if(a.next)nyi
}*/

Toplevel::Goal Chain::run(Run_info info){
	return steps[current_step].run(info);
}

Executive Chain::next_mode(Next_mode_info a){
	if(!a.autonomous) return Executive{Teleop()};
	switch(steps[current_step].done(a)){
		case Step::Status::FINISHED_SUCCESS:
			/*if(next) return next->clone();
			return unique_ptr<Mode>();*/
			current_step++;
			if(current_step==steps.size()) return next;
		case Step::Status::UNFINISHED:
			return Executive{*this};
		case Step::Status::FINISHED_FAILURE:
			nyi //TODO
		default:
			assert(0);
	}
}

void Chain::display(ostream& o)const{
	o<<"Chain(";
	o<<"current_step:"<<current_step<<" steps:"<<steps;
	o<<")";
}

bool Chain::operator==(Chain const& a)const{
	return current_step==a.current_step && steps==a.steps && next==a.next;
}

unique_ptr<Executive_interface> Chain::clone()const{
	return unique_ptr<Executive_interface>(new Chain(*this));
	//return make_unique<Chain>(step);
	//assert(0);
}

#ifdef CHAIN_TEST
int main(){
	Drive_straight a{24};
	Drive_straight b{-24};
	vector<Step> steps={Step{a},Step{b}};
	Chain c{steps,Executive{Teleop{}}};
	Chain d{c};
	auto e=d.next_mode(example((Next_mode_info*)0));
	PRINT(d.steps);
	PRINT(e);
	auto f=d.run(example((Run_info*)0));
	PRINT(f);
	d.current_step++;
	auto g=d.run(example((Run_info*)0));
	PRINT(g);
	//test_executive(Chain(Step(),Executive()));
}
#endif
