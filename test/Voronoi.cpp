
#include "Strawberry/Core/Math/Geometry/PointSet.hpp"
#include "Strawberry/Core/Math/Graph/Voronoi.hpp"
#include "Strawberry/Core/Math/Graph/Delauney.hpp"
#include "Strawberry/Core/Util/Image.hpp"
#include "canvas_ity.hpp"
#include <numbers>
#include <random>


using namespace Strawberry::Core;
using namespace Math;

static Vector<double, 2> MIN{-1000.0, -1000.0};
static Vector<double, 2> MAX{1000.0, 1000.0};
static AABB<double, 2>   BOUNDS(MIN, MAX);

struct GraphColoring
{
	bool drawNodes = true;
	bool drawEdges = true;
	bool dashEdges = false;
	float edgeWidth = 2.0f;
	float nodeRadius = 2.0f;
	float mEdgeColor[4];
	float mNodeColor[4];
};

static PointSet<double, 2> GeneratePointSet()
{
	static size_t POINT_COUNT = 128;
	PointSet<double, 2> points = PointSet<double, 2>::UniformDistribution(POINT_COUNT, MIN, MAX);
	return points.Relaxed(BOUNDS, 4);
}


static void DrawGraph(canvas_ity::canvas& canvas, const auto& graph, GraphColoring config)
{
	if (config.drawEdges)
	{
		for (auto edge : graph.Edges())
		{
			if (config.dashEdges)
			{
				constexpr float DASH_BASE = 5.0f;
				float lineDash[] = {DASH_BASE, std::numbers::phi_v<float> * DASH_BASE};
				canvas.set_line_dash(lineDash, std::extent_v<decltype(lineDash)>);
			}
			else
			{
				canvas.set_line_dash(nullptr, 0);
			}
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


static void DrawCellCenters(canvas_ity::canvas& canvas, Voronoi<Vector<double, 2>>& graph)
{
	for (auto cell : graph.Cells())
	{
		auto center = graph.GetCellAsPolygon(cell).Mean() - MIN;
		canvas.set_color(canvas_ity::fill_style, 0.2, 0.5, 0.8, 1.0);
		canvas.begin_path();
		canvas.arc(center[0], center[1], 2.0, 0, 360);
		canvas.fill();

		for (auto edge : cell.Edges())
		{
			auto a = graph.GetGraph().GetValue(edge.A()) - MIN;
			auto b = graph.GetGraph().GetValue(edge.B()) - MIN;

			a = a + (center - a).WithLength(8.0);
			b = b + (center - b).WithLength(8.0);

			auto mid = (a + b) * 0.5;
			auto perp = (b - a).Perpendicular();
			auto towardsCenter = mid + perp.WithLength(4.0);

			canvas.set_line_dash(nullptr, 0);
			canvas.set_line_width(1.0);
			canvas.set_color(canvas_ity::stroke_style, 0.5, 0.3, 0.8, 1.0);
			canvas.begin_path();
			canvas.move_to(a[0], a[1]);
			canvas.line_to(b[0], b[1]);
			canvas.stroke();
			canvas.begin_path();
			canvas.set_line_width(4.0);
			canvas.move_to(mid[0], mid[1]);
			canvas.line_to(towardsCenter[0], towardsCenter[1]);
			canvas.stroke();
		}
	}
}




int main()
{
	GraphColoring mainColoring { .drawEdges = true, .dashEdges = true, .edgeWidth = 1.0f, .mEdgeColor{0.7f, 0.7f, 0.7f, 1.0f}, .mNodeColor{0.7f, 0.0f, 0.0f, 1.0f} };
	GraphColoring voronoiColoring { .drawNodes = true, .mEdgeColor{0.0f, 0.8f, 0.8f, 1.0f}, .mNodeColor{0.0f, 1.0f, 0.0f, 1.0f} };

	PointSet<double, 2> pointSet = GeneratePointSet();

	auto builder = Delaunay<Vector<double, 2>>::Builder({MIN, MAX});
	for (const auto& point : pointSet)
	{
		builder.AddNode(point);
	}

	auto delaunay = builder.Build();
	auto voronoi = Voronoi<Vector<double, 2>>::Builder(delaunay).Build();
	auto span = MAX - MIN;

	canvas_ity::canvas context(span[0], span[1]);
	Image<PixelRGBA> image(span.AsType<unsigned int>());

	DrawGraph(context, delaunay.GetGraph(), mainColoring);
	DrawGraph(context, voronoi.GetGraph(), voronoiColoring);
	DrawCellCenters(context, voronoi);

	context.get_image_data((unsigned char*) image.Data(), image.Width(), image.Height(), image.Width() * decltype(image)::PixelType::Size, 0, 0);
	image.Save(fmt::format("voronoi_output.png"));


	return 0;
}
