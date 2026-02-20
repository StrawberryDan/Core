#include "Strawberry/Core/Math/Geometry/PointSet.hpp"
#include "Strawberry/Core/Math/Geometry/Delauney.hpp"
#include "Strawberry/Core/Util/Image.hpp"
#include "canvas_ity.hpp"
#include <random>


using namespace Strawberry::Core;
using namespace Math;

static PointSet<double, 2> GeneratePointSet()
{
	static size_t POINT_COUNT = 32;
	PointSet<double, 2> points;

	std::random_device rng;
	std::uniform_real_distribution<double> dist(0.0, 1000.0);

	for (int i = 0; i < POINT_COUNT; i++)
	{
		Vector<double, 2> v(dist(rng), dist(rng));
		points.Add(v.AsType<int>().AsType<double>());
	}
	return points;
}

int main()
{
	PointSet<double, 2> pointSet = GeneratePointSet();

	Delauney delauney(Vector{0., 0.}, Vector{1000., 1000.});

	auto drawGraph = [&](int i) {
		auto min = delauney.GetMin();
		auto max = delauney.GetMax();
		auto span = max - min;

		Image<PixelRGBA> image(span.AsType<unsigned int>());
		canvas_ity::canvas context(span[0], span[1]);

		for (auto edge : delauney.Edges())
		{
			context.set_line_width(8.0);
			Vector<double, 2> posA = delauney.GetValue(edge.nodes[0]) - min;
			Vector<double, 2> posB = delauney.GetValue(edge.nodes[1]) - min;
			context.set_color(canvas_ity::brush_type::stroke_style, 1.0f, 1.0f, 1.0f, 1.0f);
			context.begin_path();
			context.move_to(posA[0], posA[1]);
			context.line_to(posB[0], posB[1]);
			context.stroke();
		}


		for (auto node : delauney.Nodes())
		{
			auto pos = delauney.GetValue(node) - min;
			context.set_line_width(8.0);
			context.set_color(canvas_ity::brush_type::fill_style, 1.0f, 0.0f, 0.0f, 1.0f);
			context.begin_path();
			context.arc(pos[0], pos[1], 20, 0, 360);
			context.fill();

			context.fill_text(std::to_string(node).c_str(), pos[0], pos[1]);
		}

		context.get_image_data((unsigned char*) image.Data(), image.Width(), image.Height(), image.Width() * decltype(image)::PixelType::Size, 0, 0);
		image.Save(fmt::format("delauney_output_{}.png", i));
	};


	int i = 0;
	for (auto point : pointSet.Points())
	{
		delauney.AddNode(point);
		drawGraph(i++);
	}


	return 0;
}
