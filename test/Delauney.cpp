
#include "Strawberry/Core/Math/Geometry/PointSet.hpp"
#include "Strawberry/Core/Math/Geometry/Voronoi.hpp"
#include "Strawberry/Core/Util/Image.hpp"
#include "canvas_ity.hpp"
#include <random>


using namespace Strawberry::Core;
using namespace Math;

static Vector<double, 2> MIN{0.0, 0.0};
static Vector<double, 2> MAX{1000.0, 1000.0};

struct GraphColoring
{
	float edgeWidth = 2.0f;
	float nodeRadius = 4.0f;
	float mEdgeColor[4];
	float mNodeColor[4];
};

static PointSet<double, 2> GeneratePointSet()
{
	static size_t POINT_COUNT = 32;
	PointSet<double, 2> points;

	std::random_device rng;
	std::uniform_real_distribution<double> distX(MIN[0] + 1.0, MAX[0] - 1.0);
	std::uniform_real_distribution<double> distY(MIN[1] + 1.0, MAX[1] - 1.0);

	for (int i = 0; i < POINT_COUNT; i++)
	{
		Vector<double, 2> v(distX(rng), distY(rng));
		points.Add(v.AsType<int>().AsType<double>());
	}
	return points;
}


	static void DrawGraph(canvas_ity::canvas& canvas, const auto& graph, GraphColoring coloring)
	{
		for (auto edge : graph.Edges())
		{
			canvas.set_line_width(coloring.edgeWidth);
			Vector<double, 2> posA = graph.GetValue(edge.nodes[0]);
			Vector<double, 2> posB = graph.GetValue(edge.nodes[1]);
			canvas.set_color(canvas_ity::brush_type::stroke_style, coloring.mEdgeColor[0], coloring.mEdgeColor[1], coloring.mEdgeColor[2], coloring.mEdgeColor[3]);
			canvas.begin_path();
			canvas.move_to(posA[0], posA[1]);
			canvas.line_to(posB[0], posB[1]);
			canvas.stroke();
		}


		for (auto node : graph.Nodes())
		{
			auto pos = graph.GetValue(node);
			canvas.set_line_width(8.0);
			canvas.set_color(canvas_ity::brush_type::fill_style, coloring.mNodeColor[0], coloring.mNodeColor[1], coloring.mNodeColor[2], coloring.mNodeColor[3]);
			canvas.begin_path();
			canvas.arc(pos[0], pos[1], coloring.nodeRadius, 0, 360);
			canvas.fill();
		}
	}




int main()
{
	GraphColoring mainColoring { .mEdgeColor{1.0f, 1.0f, 1.0f, 1.0f}, .mNodeColor{1.0f, 0.0f, 0.0f, 1.0f} };
	GraphColoring voronoiColoring { .mEdgeColor{0.0f, 1.0f, 1.0f, 1.0f}, .mNodeColor{0.0f, 1.0f, 0.0f, 1.0f} };

	PointSet<double, 2> pointSet = GeneratePointSet();

	Delauney<Vector<double, 2>> delauney(MIN, MAX);
	auto min = delauney.GetMin();
	auto max = delauney.GetMax();
	auto span = max - min;

	canvas_ity::canvas context(span[0], span[1]);
	Image<PixelRGBA> image(span.template AsType<unsigned int>());

	for (auto point : pointSet.Points())
	{
		delauney.AddNode(point);
	}

	auto voronoi = Voronoi<Vector<double, 2>>::From(delauney);

	DrawGraph(context, voronoi.Triangulation(), mainColoring);
	DrawGraph(context, voronoi.Edges(), voronoiColoring);

	context.get_image_data((unsigned char*) image.Data(), image.Width(), image.Height(), image.Width() * decltype(image)::PixelType::Size, 0, 0);
	image.Save(fmt::format("delauney_output.png"));


	return 0;
}
