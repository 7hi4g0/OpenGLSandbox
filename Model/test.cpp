#include <model.h>

#define set_insert(set, elem)												\
	if (set.find(elem) == set.end()) {									\
		std::cout << #elem " not found! Inserting..." << std::endl;			\
	}																	\
	std::cout << "Inserted: " << set.insert(elem).second << std::endl;	\

int main(void) {
	Model model;

	PositionPtr p1(new Position{{-1.0, -1.0, 1.0}});
	PositionPtr p2(new Position{{-1.0, -1.0, -1.0}});
	PositionPtr p3(new Position{{1.0, -1.0, -1.0}});
	PositionPtr p4(new Position{{1.0, -1.0, 1.0}});
	PositionPtr p5(new Position{{-1.0, 1.0, 1.0}});
	PositionPtr p6(new Position{{-1.0, 1.0, -1.0}});
	PositionPtr p7(new Position{{1.0, 1.0, -1.0}});
	PositionPtr p8(new Position{{1.0, 1.0, 1.0}});

	EdgePtr e1(new Edge{p5, p6});
	EdgePtr e2(new Edge{p6, p2});
	EdgePtr e3(new Edge{p2, p1});
	EdgePtr e4(new Edge{p1, p5});
	EdgePtr e5(new Edge{p6, p7});
	EdgePtr e6(new Edge{p7, p3});
	EdgePtr e7(new Edge{p3, p2});
	EdgePtr e8(new Edge{p2, p6});
	EdgePtr e9(new Edge{p7, p8});
	EdgePtr e10(new Edge{p8, p4});
	EdgePtr e11(new Edge{p4, p3});
	EdgePtr e12(new Edge{p3, p7});
	EdgePtr e13(new Edge{p8, p5});
	EdgePtr e14(new Edge{p5, p1});
	EdgePtr e15(new Edge{p1, p4});
	EdgePtr e16(new Edge{p4, p8});
	EdgePtr e17(new Edge{p1, p2});
	EdgePtr e18(new Edge{p2, p3});
	EdgePtr e19(new Edge{p3, p4});
	EdgePtr e20(new Edge{p4, p1});
	EdgePtr e21(new Edge{p8, p7});
	EdgePtr e22(new Edge{p7, p6});
	EdgePtr e23(new Edge{p6, p5});
	EdgePtr e24(new Edge{p5, p8});

	model.pos.insert(p1);
	model.pos.insert(p2);
	model.pos.insert(p3);
	model.pos.insert(p4);
	model.pos.insert(p5);
	model.pos.insert(p6);
	model.pos.insert(p7);
	model.pos.insert(p8);

	std::set<Edge> edges;

	set_insert(model.edges, e1);
	set_insert(model.edges, e2);
	set_insert(model.edges, e3);
	set_insert(model.edges, e4);
	set_insert(model.edges, e5);
	set_insert(model.edges, e6);
	set_insert(model.edges, e7);
	set_insert(model.edges, e8);
	set_insert(model.edges, e9);
	set_insert(model.edges, e10);
	set_insert(model.edges, e11);
	set_insert(model.edges, e12);
	set_insert(model.edges, e13);
	set_insert(model.edges, e14);
	set_insert(model.edges, e15);
	set_insert(model.edges, e16);
	set_insert(model.edges, e17);
	set_insert(model.edges, e18);
	set_insert(model.edges, e19);
	set_insert(model.edges, e20);
	set_insert(model.edges, e21);
	set_insert(model.edges, e22);
	set_insert(model.edges, e23);
	set_insert(model.edges, e24);

	set_insert(edges, *e1);
	set_insert(edges, *e2);
	set_insert(edges, *e3);
	set_insert(edges, *e4);
	set_insert(edges, *e5);
	set_insert(edges, *e6);
	set_insert(edges, *e7);
	set_insert(edges, *e8);
	set_insert(edges, *e9);
	set_insert(edges, *e10);
	set_insert(edges, *e11);
	set_insert(edges, *e12);
	set_insert(edges, *e13);
	set_insert(edges, *e14);
	set_insert(edges, *e15);
	set_insert(edges, *e16);
	set_insert(edges, *e17);
	set_insert(edges, *e18);
	set_insert(edges, *e19);
	set_insert(edges, *e20);
	set_insert(edges, *e21);
	set_insert(edges, *e22);
	set_insert(edges, *e23);
	set_insert(edges, *e24);

	PositionCompare p_comp;

	std::cout << "p1 == p2: " << (!p_comp(p1, p2) && !p_comp(p2, p1)) << std::endl;
	std::cout << "p1 == p3: " << (!p_comp(p1, p3) && !p_comp(p3, p1)) << std::endl;
	std::cout << "p1 == p4: " << (!p_comp(p1, p4) && !p_comp(p4, p1)) << std::endl;
	std::cout << "p1 == p5: " << (!p_comp(p1, p5) && !p_comp(p5, p1)) << std::endl;
	std::cout << "p1 == p6: " << (!p_comp(p1, p6) && !p_comp(p6, p1)) << std::endl;
	std::cout << "p1 == p7: " << (!p_comp(p1, p7) && !p_comp(p7, p1)) << std::endl;
	std::cout << "p1 == p8: " << (!p_comp(p1, p8) && !p_comp(p8, p1)) << std::endl;
	std::cout << "p2 == p3: " << (!p_comp(p2, p3) && !p_comp(p3, p2)) << std::endl;
	std::cout << "p2 == p4: " << (!p_comp(p2, p4) && !p_comp(p4, p2)) << std::endl;
	std::cout << "p2 == p5: " << (!p_comp(p2, p5) && !p_comp(p5, p2)) << std::endl;
	std::cout << "p2 == p6: " << (!p_comp(p2, p6) && !p_comp(p6, p2)) << std::endl;
	std::cout << "p2 == p7: " << (!p_comp(p2, p7) && !p_comp(p7, p2)) << std::endl;
	std::cout << "p2 == p8: " << (!p_comp(p2, p8) && !p_comp(p8, p2)) << std::endl;
	std::cout << "p3 == p4: " << (!p_comp(p3, p4) && !p_comp(p4, p3)) << std::endl;
	std::cout << "p3 == p5: " << (!p_comp(p3, p5) && !p_comp(p5, p3)) << std::endl;
	std::cout << "p3 == p6: " << (!p_comp(p3, p6) && !p_comp(p6, p3)) << std::endl;
	std::cout << "p3 == p7: " << (!p_comp(p3, p7) && !p_comp(p7, p3)) << std::endl;
	std::cout << "p3 == p8: " << (!p_comp(p3, p8) && !p_comp(p8, p3)) << std::endl;
	std::cout << "p4 == p5: " << (!p_comp(p4, p5) && !p_comp(p5, p4)) << std::endl;
	std::cout << "p4 == p6: " << (!p_comp(p4, p6) && !p_comp(p6, p4)) << std::endl;
	std::cout << "p4 == p7: " << (!p_comp(p4, p7) && !p_comp(p7, p4)) << std::endl;
	std::cout << "p4 == p8: " << (!p_comp(p4, p8) && !p_comp(p8, p4)) << std::endl;
	std::cout << "p5 == p6: " << (!p_comp(p5, p6) && !p_comp(p6, p5)) << std::endl;
	std::cout << "p5 == p7: " << (!p_comp(p5, p7) && !p_comp(p7, p5)) << std::endl;
	std::cout << "p5 == p8: " << (!p_comp(p5, p8) && !p_comp(p8, p5)) << std::endl;
	std::cout << "p6 == p7: " << (!p_comp(p6, p7) && !p_comp(p7, p6)) << std::endl;
	std::cout << "p6 == p8: " << (!p_comp(p6, p8) && !p_comp(p8, p6)) << std::endl;
	std::cout << "p7 == p8: " << (!p_comp(p7, p8) && !p_comp(p8, p7)) << std::endl;

	EdgeCompare comp;

	std::cout << "e1 == e23: " << (!comp(e1, e23) && !comp(e23, e1)) << std::endl;
	std::cout << "e2 == e8: " << (!comp(e2, e8) && !comp(e8, e2)) << std::endl;
	std::cout << "e3 == e17: " << (!comp(e3, e17) && !comp(e17, e3)) << std::endl;
	std::cout << "e4 == e14: " << (!comp(e4, e14) && !comp(e14, e4)) << std::endl;
	std::cout << "e5 == e22: " << (!comp(e5, e22) && !comp(e22, e5)) << std::endl;
	std::cout << "e6 == e12: " << (!comp(e6, e12) && !comp(e12, e6)) << std::endl;
	std::cout << "e7 == e18: " << (!comp(e7, e18) && !comp(e18, e7)) << std::endl;
	std::cout << "e9 == e21: " << (!comp(e9, e21) && !comp(e21, e9)) << std::endl;
	std::cout << "e10 == e16: " << (!comp(e10, e16) && !comp(e16, e10)) << std::endl;
	std::cout << "e11 == e19: " << (!comp(e11, e19) && !comp(e19, e11)) << std::endl;
	std::cout << "e13 == e24: " << (!comp(e13, e24) && !comp(e24, e13)) << std::endl;
	std::cout << "e15 == e20: " << (!comp(e15, e20) && !comp(e20, e15)) << std::endl;

	std::cout << "Points: " << model.pos.size() << std::endl;
	std::cout << "Edges: " << model.edges.size() << std::endl;
	std::cout << "Other edges: " << edges.size() << std::endl;

	return 0;
}
