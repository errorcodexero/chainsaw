#include "chain2.h"
#include "teleop.h"

using namespace std;

Chain2::Chain2(vector<Step> s, Executive n):current_step(0),steps(s),next(n){}

/*Chain2::Chain2(Chain2 const& a):step(a.step){
	if(a.next)nyi
}*/

Toplevel::Goal Chain2::run(Run_info info){
	return steps[current_step].run(info);
}

Executive Chain2::next_mode(Next_mode_info a){
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

void Chain2::display(ostream& o)const{
	o<<"Chain2(";
	o<<"current_step:"<<current_step<<" steps:"<<steps;
	o<<")";
}

bool Chain2::operator==(Chain2 const&)const{
	return 1;
}

unique_ptr<Executive_interface> Chain2::clone()const{
	return unique_ptr<Executive_interface>(new Chain2(*this));
	//return make_unique<Chain2>(step);
	//assert(0);
}

#ifdef CHAIN2_TEST
int main(){
	Drive_straight a{24};
	Drive_straight b{-24};
	vector<Step> steps={Step{a},Step{b}};
	Chain2 c{steps,Executive{Teleop{}}};
	Chain2 d{c};
	auto e=d.next_mode(example((Next_mode_info*)0));
	PRINT(d.steps);
	PRINT(e);
	auto f=d.run(example((Run_info*)0));
	PRINT(f);
	d.current_step++;
	auto g=d.run(example((Run_info*)0));
	PRINT(g);
	//test_executive(Chain2(Step(),Executive()));
}
#endif
