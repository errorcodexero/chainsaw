#include "chain.h"
#include "teleop.h"

using namespace std;

Chain::Chain(Step a,Executive next1):step(a),next(next1){}

/*Chain::Chain(Chain const& a):step(a.step){
	if(a.next)nyi
}*/

Toplevel::Goal Chain::run(Run_info){
	return Toplevel::Goal{};
}

Executive Chain::next_mode(Next_mode_info a){
	if(step.done(a)){
		/*if(next) return next->clone();
		return unique_ptr<Mode>();*/
		return next;
	}
	return Executive{*this};
}

void Chain::display(ostream& o)const{
	o<<"Chain(";
	o<<"step:"<<step<<" next:"<<next;
	o<<")";
}

bool Chain::operator==(Chain const&)const{
	return 1;
}

unique_ptr<Executive_interface> Chain::clone()const{
	return unique_ptr<Executive_interface>(new Chain(*this));
	//return make_unique<Chain>(step);
	//assert(0);
}

#ifdef CHAIN_TEST
int main(){
	Drive_straight a;
	Chain b{Step{a},Executive{Teleop{}}};
	Chain c{b};
	auto d=c.next_mode(example((Next_mode_info*)0));
	PRINT(c.step);
	PRINT(d);
	//test_executive(Chain(Step(),Executive()));
}
#endif
