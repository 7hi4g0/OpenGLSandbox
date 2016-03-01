#include <model.h>

int main(void) {
	Model model;

	PositionPtr p1(new Position{1.0, 1.0, 1.0});
	PositionPtr p2(new Position{2.0, 2.0, 2.0});

	EdgePtr e1(new Edge{p1, p2});
	EdgePtr e2(new Edge{p2, p1});

	model.pos.insert(p1);
	model.pos.insert(p2);

	model.edges.insert(e1);
	model.edges.insert(e2);

	std::cout << "Points: " << model.pos.size() << std::endl;
	std::cout << "Edges: " << model.edges.size() << std::endl;

	return 0;
}
