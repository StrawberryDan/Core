
#include "Strawberry/Core/Math/Geometry/PointSet.hpp"
#include "Strawberry/Core/Math/Graph/Delauney.hpp"
#include "Strawberry/Core/Util/Image.hpp"
#include "canvas_ity.hpp"
#include <random>


using namespace Strawberry::Core;
using namespace Math;

static Vector<double, 2> MIN{0.0, 0.0};
static Vector<double, 2> MAX{1000.0, 1000.0};

struct GraphColoring
{
	bool drawNodes = true;
	bool drawEdges = true;
	float edgeWidth = 2.0f;
	float nodeRadius = 4.0f;
	float mEdgeColor[4];
	float mNodeColor[4];
};

static PointSet<double, 2> GeneratePointSet()
{
	static size_t POINT_COUNT = 128;
	PointSet<double, 2> points;

	std::random_device rng;
	std::uniform_real_distribution<double> distX(MIN[0], MAX[0]);
	std::uniform_real_distribution<double> distY(MIN[1], MAX[1]);

	for (int i = 0; i < POINT_COUNT; i++)
	{
		Vector<double, 2> v(distX(rng), distY(rng));
		v = v.AsType<int>().AsType<double>();
		points.Add(v);
	}
	return points;
}


static void DrawGraph(canvas_ity::canvas& canvas, const auto& graph, GraphColoring config)
{
	if (config.drawEdges)
	{
		for (auto edge : graph.Edges())
		{
			canvas.set_line_width(config.edgeWidth);
			Vector<double, 2> posA = graph.GetValue(edge.nodes[0]) - MIN;
			Vector<double, 2> posB = graph.GetValue(edge.nodes[1]) - MIN;
			canvas.set_color(canvas_ity::brush_type::stroke_style, config.mEdgeColor[0], config.mEdgeColor[1], config.mEdgeColor[2], config.mEdgeColor[3]);
			canvas.begin_path();
			canvas.move_to(posA[0], posA[1]);
			canvas.line_to(posB[0], posB[1]);
			canvas.stroke();
		}
	}


	if (config.drawNodes)
	{
		for (auto node : graph.NodeIndices())
		{
			auto pos = graph.GetValue(node) - MIN;
			canvas.set_line_width(8.0);
			canvas.set_color(canvas_ity::brush_type::fill_style, config.mNodeColor[0], config.mNodeColor[1], config.mNodeColor[2], config.mNodeColor[3]);
			canvas.begin_path();
			canvas.arc(pos[0], pos[1], config.nodeRadius, 0, 360);
			canvas.fill();
		}
	}
}




int main()
{
	GraphColoring mainColoring { .mEdgeColor{1.0f, 1.0f, 1.0f, 1.0f}, .mNodeColor{1.0f, 0.0f, 0.0f, 1.0f} };

	PointSet<double, 2> pointSet = GeneratePointSet();

	auto builder = Delaunay<Vector<double, 2>>::Builder({MIN, MAX});
	for (const auto& point : pointSet)
	{
		builder.AddNode(point);
	}

	auto delaunay = builder.Build();
	auto span = MAX - MIN;

	canvas_ity::canvas context(span[0], span[1]);
	Image<PixelRGBA> image(span.template AsType<unsigned int>());

	DrawGraph(context, delaunay.GetGraph(), mainColoring);

	context.get_image_data((unsigned char*) image.Data(), image.Width(), image.Height(), image.Width() * decltype(image)::PixelType::Size, 0, 0);
	image.Save(fmt::format("delaunay_output.png"));


	return 0;
}
