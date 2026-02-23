#include "Strawberry/Core/Math/Geometry/LineSegment.hpp"
#include "Strawberry/Core/Math/Geometry/PointSet.hpp"
#include "Strawberry/Core/Math/Geometry/Delauney.hpp"
#include "Strawberry/Core/Util/Image.hpp"
#include "canvas_ity.hpp"
#include <random>


using namespace Strawberry::Core;
using namespace Math;

struct GraphColoring
{
	float edgeWidth = 1.0f;
	float nodeRadius = 4.0f;
	float mEdgeColor[4];
	float mNodeColor[4];
};

static PointSet<double, 2> GeneratePointSet()
{
	static size_t POINT_COUNT = 512;
	PointSet<double, 2> points;

	std::random_device rng;
	std::uniform_real_distribution<double> dist(0.0, 10000.0);

	for (int i = 0; i < POINT_COUNT; i++)
	{
		Vector<double, 2> v(dist(rng), dist(rng));
		points.Add(v.AsType<int>().AsType<double>());
	}
	return points;
}



int main()
{
	GraphColoring mainColoring { .mEdgeColor{1.0f, 1.0f, 1.0f, 1.0f}, .mNodeColor{1.0f, 0.0f, 0.0f, 1.0f} };
	GraphColoring voronoiColoring { .mEdgeColor{0.0f, 1.0f, 1.0f, 1.0f}, .mNodeColor{0.0f, 1.0f, 0.0f, 1.0f} };

	PointSet<double, 2> pointSet = GeneratePointSet();

	Delauney delauney(Vector{0., 0.}, Vector{10000., 10000.});
	auto min = delauney.GetMin();
	auto max = delauney.GetMax();
	auto span = max - min;

	canvas_ity::canvas context(span[0], span[1]);
	Image<PixelRGBA> image(span.template AsType<unsigned int>());


	auto drawGraph = [&](canvas_ity::canvas& canvas, const auto& graph, GraphColoring coloring, int i) {
		for (auto edge : graph.Edges())
		{
			context.set_line_width(coloring.edgeWidth);
			Vector<double, 2> posA = graph.GetValue(edge.nodes[0]) - min;
			Vector<double, 2> posB = graph.GetValue(edge.nodes[1]) - min;
			context.set_color(canvas_ity::brush_type::stroke_style, coloring.mEdgeColor[0], coloring.mEdgeColor[1], coloring.mEdgeColor[2], coloring.mEdgeColor[3]);
			context.begin_path();
			context.move_to(posA[0], posA[1]);
			context.line_to(posB[0], posB[1]);
			context.stroke();
		}


		for (auto node : graph.Nodes())
		{
			auto pos = graph.GetValue(node) - min;
			context.set_line_width(8.0);
			context.set_color(canvas_ity::brush_type::fill_style, coloring.mNodeColor[0], coloring.mNodeColor[1], coloring.mNodeColor[2], coloring.mNodeColor[3]);
			context.begin_path();
			context.arc(pos[0], pos[1], coloring.nodeRadius, 0, 360);
			context.fill();

			context.fill_text(std::to_string(node).c_str(), pos[0], pos[1]);
		}

	};


	for (auto point : pointSet.Points())
	{
		delauney.AddNode(point);
	}

	drawGraph(context, delauney, mainColoring, 0);
	drawGraph(context, delauney.ToVoronoi(), voronoiColoring, 1);

	context.get_image_data((unsigned char*) image.Data(), image.Width(), image.Height(), image.Width() * decltype(image)::PixelType::Size, 0, 0);
	image.Save(fmt::format("delauney_output.png"));


	return 0;
}
