#include "Strawberry/Core/Math/Geometry/Delauney.hpp"
#include "Strawberry/Core/Util/Image.hpp"
#include "canvas_ity.hpp"


using namespace Strawberry::Core;
using namespace Math;

int main()
{
	PointSet<double, 2> pointSet;

	pointSet.Add({0.0, 0.0});
	pointSet.Add({1000.0, 1000.0});
	pointSet.Add({800, 900});
	pointSet.Add({900, 800});
	pointSet.Add({700, 800});
	pointSet.Add({800, 700});

	auto delauney = Delauney<double>::FromPoints(pointSet);

	auto min = delauney.GetMinPoint();
	auto max = delauney.GetMaxPoint();
	auto span = max - min;
	span[0] += 20; span[1] += 20;

	Image<PixelRGBA> image(span.AsType<unsigned int>());

	canvas_ity::canvas context(span[0], span[1]);

	for (auto nodes : delauney.GetNodes())
	{
		auto pos = nodes.second.Offset(10, 10) - min;
		Logging::Info("Drawing node at {}, {}", pos[0], pos[1]);
		context.set_line_width(5.0);
		context.set_color(canvas_ity::brush_type::stroke_style, 1.0f, 1.0f, 1.0f, 1.0f);
		context.begin_path();
		context.arc(pos[0], pos[1], 5, 0, 360);
		context.stroke();
	}

	for (auto edge : delauney.GetEdges())
	{
		Vector<double, 2> posA = delauney.GetValue(edge.nodes[0]).Offset(10, 10) - min;
		Vector<double, 2> posB = delauney.GetValue(edge.nodes[1]).Offset(10, 10) - min;
		context.move_to(posA[0], posA[1]);
		context.line_to(posB[0], posB[1]);
		context.stroke();
	}

	context.get_image_data((unsigned char*) image.Data(), image.Width(), image.Height(), image.Width() * decltype(image)::PixelType::Size, 0, 0);


	image.Save("delauney_output.png");

	return 0;
}
