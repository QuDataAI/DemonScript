#include "SVGModule.h"
#include "ModuleManager.h"

#define SVG_PI       3.14159265358979323846   // pi

//регистрируем модуль
IMPLEMENTATION_MODULE(SVGModule, "SVG")

//регистрируем функции модуля
IMPLEMENTATION_MODULE_FUNCTION(SVGModule, SVGFuncCreate     , "create");
//IMPLEMENTATION_MODULE_FUNCTION(SVGModule, SVGFuncSave       , "save");
////IMPLEMENTATION_MODULE_FUNCTION(SVGModule, SVGFuncClear      , "clear");
////IMPLEMENTATION_MODULE_FUNCTION(SVGModule, SVGFuncColorFill  , "color_fill");
////IMPLEMENTATION_MODULE_FUNCTION(SVGModule, SVGFuncColorLine  , "color_line");
////IMPLEMENTATION_MODULE_FUNCTION(SVGModule, SVGFuncColorText  , "color_text");
////IMPLEMENTATION_MODULE_FUNCTION(SVGModule, SVGFuncWidthLine  , "width_line");
////IMPLEMENTATION_MODULE_FUNCTION(SVGModule, SVGFuncSizeFont   , "size_font");
////IMPLEMENTATION_MODULE_FUNCTION(SVGModule, SVGFuncLine       , "line");
////IMPLEMENTATION_MODULE_FUNCTION(SVGModule, SVGFuncRect       , "rect");
////IMPLEMENTATION_MODULE_FUNCTION(SVGModule, SVGFuncBox        , "box");
////IMPLEMENTATION_MODULE_FUNCTION(SVGModule, SVGFuncCircle     , "circle");
////IMPLEMENTATION_MODULE_FUNCTION(SVGModule, SVGFuncCircleFill , "circle_fill");
////IMPLEMENTATION_MODULE_FUNCTION(SVGModule, SVGFuncPoint      , "point");
//IMPLEMENTATION_MODULE_FUNCTION(SVGModule, SVGFuncArc        , "arc");
////IMPLEMENTATION_MODULE_FUNCTION(SVGModule, SVGFuncText       , "text");

//=======================================================================================
string SVGModule::color_line(ValueSVGPtr svg)
{
   return svg->_colorLine;
}
//=======================================================================================
void SVGModule::color_line(ValueSVGPtr svg, string val)
{
   svg->_colorLine = val;
}
//=======================================================================================
string SVGModule::color_fill(ValueSVGPtr svg)
{
   return svg->_colorFill;
}
//=======================================================================================
void SVGModule::color_fill(ValueSVGPtr svg, string val)
{
   svg->_colorFill = val;
}
//=======================================================================================
string SVGModule::color_text(ValueSVGPtr svg)
{
   return svg->_colorText;
}
//=======================================================================================
void SVGModule::color_text(ValueSVGPtr svg, string val)
{
   svg->_colorText = val;
}
//=======================================================================================
Float SVGModule::width_line(ValueSVGPtr svg)
{
   return svg->_widhtLine;
}
//=======================================================================================
void SVGModule::width_line(ValueSVGPtr svg, Float val)
{
   svg->_widhtLine = val;
}
//=======================================================================================
Float SVGModule::size_font(ValueSVGPtr svg)
{
   return svg->_sizeFont;
}
//=======================================================================================
void SVGModule::size_font(ValueSVGPtr svg, Float val)
{
   svg->_sizeFont = val;
}
//=======================================================================================
void SVGModule::line(ValueSVGPtr svg, Float x1, Float y1, Float x2, Float y2)
{
   svg->_val += "<line x1=\"" + to_stringI(x1) + "\" y1=\"" + to_stringI(y1) + "\" x2=\"" + to_stringI(x2) + "\" y2=\"" + to_stringI(y2) + "\" stroke=\"" + svg->_colorLine + "\" stroke-width=\"" + to_stringI(svg->_widhtLine) + "\"/>\n";
}
//=======================================================================================
void SVGModule::rect(ValueSVGPtr svg, Float x, Float y, Float w, Float h, Float r)
{
   svg->_val += "<rect x=\"" + to_stringI(x - w / 2) + "\" y=\"" + to_stringI(y - h / 2) + "\" width=\"" + to_stringI(w) + "\" height=\"" + to_stringI(h) + "\" " + (r ? "rx=\"" + to_stringI(r) + "\" ry=\"" + to_stringI(r) + "\" " : "") + "stroke=\"" + svg->_colorLine + "\" stroke-width=\"" + to_stringI(svg->_widhtLine) + "\" fill=\"none\" />\n";
}
//=======================================================================================
void SVGModule::box(ValueSVGPtr svg, Float x, Float y, Float w, Float h, Float r)
{
   svg->_val += "<rect x=\"" + to_stringI(x - w / 2) + "\" y=\"" + to_stringI(y - h / 2) + "\" width=\"" + to_stringI(w) + "\" height=\"" + to_stringI(h) + "\" " + (r ? "rx=\"" + to_stringI(r) + "\" ry=\"" + to_stringI(r) + "\" " : "") + "stroke=\"" + svg->_colorLine + "\" stroke-width=\"" + to_stringI(svg->_widhtLine) + "\" fill=\"" + svg->_colorFill + "\" />\n";
}
//=======================================================================================
void SVGModule::circle(ValueSVGPtr svg, Float x, Float y, Float r)
{
   svg->_val += "<circle cx=\"" + to_stringI(x) + "\" cy=\"" + to_stringI(y) + "\" r=\"" + to_stringI(r) + "\" stroke=\"" + svg->_colorLine + "\" fill=\"none\"/>\n";
}
//=======================================================================================
void SVGModule::circle_fill(ValueSVGPtr svg, Float x, Float y, Float r)
{
   svg->_val += "<circle cx=\"" + to_stringI(x) + "\" cy=\"" + to_stringI(y) + "\" r=\"" + to_stringI(r) + "\" stroke=\"" + svg->_colorLine + "\" fill=\"" + svg->_colorFill + "\"/>\n";
}
//=======================================================================================
void SVGModule::point(ValueSVGPtr svg, Float x, Float y, Float r)
{
   svg->_val += "<circle cx=\"" + to_stringI(x) + "\" cy=\"" + to_stringI(y) + "\" r=\"" + to_stringI(r) + "\" stroke=\"" + svg->_colorFill + "\" fill=\"" + svg->_colorFill + "\"/>\n";
}
//=======================================================================================
void SVGModule::arc(ValueSVGPtr svg, Float x, Float y, Float r, Float a1, Float a2)
{
   Float radians1 = (- a1) * SVG_PI / 180.0;
   Float radians2 = (- a2) * SVG_PI / 180.0;
   Float x1 = x + (r * cos(radians1));
   Float y1 = y + (r * sin(radians1));
   Float x2 = x + (r * cos(radians2));
   Float y2 = y + (r * sin(radians2));

   string flag = radians2 - radians1 <= 180 ? "0" : "1";
   string d = "M " + to_stringI(x1) + " " + to_stringI(y1) + " A " + to_stringI(r) + " " + to_stringI(r) + " 0 " + flag + " 0 " + to_stringI(x2) + " " + to_stringI(y2);

   svg->_val += "<path fill=\"" + svg->_colorFill + "\" stroke=\"" + svg->_colorLine + "\" stroke-width=\"" + to_stringI(svg->_widhtLine) + "\" d = \"" + d + "\" />\n";
}
//=======================================================================================
void SVGModule::text(ValueSVGPtr svg, string txt, Float x, Float y)
{
   svg->_val += "<text x=\"" + to_stringI(x) + "\" y=\"" + to_stringI(y) + "\" alignment-baseline=\"middle\" fill=\"" + svg->_colorText + "\" " + "font-size=\"" + to_stringI(svg->_sizeFont) + "px\" >" + txt + "</text>\n";
}
//=======================================================================================
void SVGModule::polygon(ValueSVGPtr svg, vector<Float> &x, vector<Float> &y, bool fill, vector<size_t> &way)
{
   if (x.size() == 0 || x.size() != y.size())
      return;

   string st = "";
   if (way.size() > 0)
   {
      string first = "";
      for (size_t i = 0; i < way.size(); i++)
      {
         if (way[i] >= x.size())
            continue;
         st += to_stringI(x[way[i]]) + "," + to_stringI(y[way[i]]) + " ";
         if (first.length() == 0)
            first = to_stringI(x[way[i]]) + "," + to_stringI(y[way[i]]);
      }
      if (first.length() > 0)
         st += first;
   }
   else
   {
      for (size_t i = 0; i < x.size(); i++)
      {
         st += to_stringI(x[i]) + "," + to_stringI(y[i]) + " ";
      }
      st += to_stringI(x[0]) + "," + to_stringI(y[0]);
   }

   svg->_val += "<polyline points=\"" + st + "\"" + (fill ? " fill=\"" + svg->_colorFill + "\"" : " fill=\"transparent\"") + " stroke=\"" + svg->_colorLine + "\" stroke-width=\"" + to_stringI(svg->_widhtLine) + "\" />\n";
}
//=======================================================================================
void SVGModule::save(ValueSVGPtr svg, string fname, Float w, Float h)
{
   ofstream f(_script->workPath(fname), std::ofstream::out);
   f << ("<svg width=\"" + to_string(w) + "px\" height=\"" + to_string(h) + "px\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\" > \n");
   f << svg->_val;
   f << "</svg>";
   f.close();
}
//=======================================================================================
void SVGModule::transf_beg(ValueSVGPtr svg, Float dx, Float dy, Float phi, Float sx, Float sy)
{
   svg->_val += "<g transform=\"translate(" + to_string(dx) + " " + to_string(dy) + ")" +
      (phi != 0 ? " rotate(" + to_string(phi * 180 / SVG_PI) + ")" : "")
      + ((sx != 1 || sy != 1) ? " scale(" + to_string(sx) + " " + to_string(sy) + ")" : "")
      + "\">\n";
}
//=======================================================================================
void SVGModule::transf_end(ValueSVGPtr svg)
{
   svg->_val += "</g>\n";
}
//=======================================================================================
void SVGModule::transf_all(ValueSVGPtr svg, Float dx, Float dy, Float phi, Float sx, Float sy)
{
   svg->_val = "<g transform=\"translate(" + to_string(dx) + " " + to_string(dy) + ")"
      + (phi != 0 ? " rotate(" + to_string(phi * 180 / SVG_PI) + ")" : "")
      + ((sx != 1 || sy != 1) ? " scale(" + to_string(sx) + " " + to_string(sy) + ")" : "")
      + "\">\n"
      + svg->_val
      + "</g>\n";
}
//=======================================================================================
//=======================================================================================
//!                
//
//=======================================================================================
//! Создать пустую картинку
//
SVGFuncCreate::SVGFuncCreate()
{
}
//=======================================================================================
//! Открывает файл
//
Value SVGFuncCreate::run()
{
   return ValueSVG("");
}
//=======================================================================================
FunSVGColorLine::FunSVGColorLine(ScriptPtr script)
{
   _name = "color_line";
   _script = script;
   _num_calls = 0;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_SVG);
}
//=======================================================================================
Value FunSVGColorLine::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   SVGModule * module = (SVGModule *)(ModuleManager::instance()->getModule("SVG"));

   if (!module)
      return _objVal;

   ValueSVGPtr svg = SPTR_DCAST(ValueSVG, _objVal.ptr());

   if (_vals.size() > 0)
   {
      module->color_line(svg, _vals[0].get_Str());
   }

   return module->color_line(svg);
}
//=======================================================================================
FunSVGColorFill::FunSVGColorFill(ScriptPtr script)
{
   _name = "color_fill";
   _script = script;
   _num_calls = 0;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_SVG);
}
//=======================================================================================
Value FunSVGColorFill::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   SVGModule * module = (SVGModule *)(ModuleManager::instance()->getModule("SVG"));

   if (!module)
      return _objVal;

   ValueSVGPtr svg = SPTR_DCAST(ValueSVG, _objVal.ptr());

   if (_vals.size() > 0)
   {
      module->color_fill(svg, _vals[0].get_Str());
   }

   return module->color_fill(svg);
}
//=======================================================================================
FunSVGColorText::FunSVGColorText(ScriptPtr script)
{
   _name = "color_text";
   _script = script;
   _num_calls = 0;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_SVG);
}
//=======================================================================================
Value FunSVGColorText::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   SVGModule * module = (SVGModule *)(ModuleManager::instance()->getModule("SVG"));

   if (!module)
      return _objVal;

   ValueSVGPtr svg = SPTR_DCAST(ValueSVG, _objVal.ptr());

   if (_vals.size() > 0)
   {
      module->color_text(svg, _vals[0].get_Str());
   }

   return module->color_text(svg);
}
//=======================================================================================
FunSVGWidthLine::FunSVGWidthLine(ScriptPtr script)
{
   _name = "width_line";
   _script = script;
   _num_calls = 0;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_SVG);
}
//=======================================================================================
Value FunSVGWidthLine::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   SVGModule * module = (SVGModule *)(ModuleManager::instance()->getModule("SVG"));

   if (!module)
      return _objVal;

   ValueSVGPtr svg = SPTR_DCAST(ValueSVG, _objVal.ptr());

   if (_vals.size() > 0)
   {
      module->width_line(svg, _vals[0].get_Float());
   }

   return module->width_line(svg);
}
//=======================================================================================
FunSVGSizeFont::FunSVGSizeFont(ScriptPtr script)
{
   _name = "size_font";
   _script = script;
   _num_calls = 0;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_SVG);
}
//=======================================================================================
Value FunSVGSizeFont::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   SVGModule * module = (SVGModule *)(ModuleManager::instance()->getModule("SVG"));

   if (!module)
      return _objVal;

   ValueSVGPtr svg = SPTR_DCAST(ValueSVG, _objVal.ptr());

   if (_vals.size() > 0)
   {
      module->size_font(svg, _vals[0].get_Float());
   }

   return module->size_font(svg);
}
//=======================================================================================
FunSVGLine::FunSVGLine(ScriptPtr script)
{
   _name = "line";
   _script = script;
   _num_calls = 0;

   _objRequired = true;
   _objTypes.push_back(ValueBase::_SVG);

   _minArgs = 4;
   _argTypes.resize(4);
   _argTypes[0].push_back(ValueBase::_FLOAT);
   _argTypes[1].push_back(ValueBase::_FLOAT);
   _argTypes[2].push_back(ValueBase::_FLOAT);
   _argTypes[3].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
Value FunSVGLine::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   SVGModule * module = (SVGModule *)(ModuleManager::instance()->getModule("SVG"));

   if (!module)
      return _objVal;

   ValueSVGPtr svg = SPTR_DCAST(ValueSVG, _objVal.ptr());
   module->line(svg, _vals[0].get_Float(), _vals[1].get_Float(), _vals[2].get_Float(), _vals[3].get_Float());

   return _objVal;
}
//=======================================================================================
FunSVGRect::FunSVGRect(ScriptPtr script)
{
   _name = "rect";
   _script = script;
   _num_calls = 0;

   _objRequired = true;
   _objTypes.push_back(ValueBase::_SVG);

   _minArgs = 4;
   _argTypes.resize(5);
   _argTypes[0].push_back(ValueBase::_FLOAT);
   _argTypes[1].push_back(ValueBase::_FLOAT);
   _argTypes[2].push_back(ValueBase::_FLOAT);
   _argTypes[3].push_back(ValueBase::_FLOAT);
   _argTypes[4].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
Value FunSVGRect::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   SVGModule * module = (SVGModule *)(ModuleManager::instance()->getModule("SVG"));

   if (!module)
      return _objVal;

   ValueSVGPtr svg = SPTR_DCAST(ValueSVG, _objVal.ptr());
   module->rect(svg, _vals[0].get_Float(), _vals[1].get_Float(), _vals[2].get_Float(), _vals[3].get_Float(), _vals.size() > 4 ? _vals[4].get_Float() : 0);

   return _objVal;
}
//=======================================================================================
FunSVGBox::FunSVGBox(ScriptPtr script)
{
   _name = "box";
   _script = script;
   _num_calls = 0;

   _objRequired = true;
   _objTypes.push_back(ValueBase::_SVG);

   _minArgs = 4;
   _argTypes.resize(5);
   _argTypes[0].push_back(ValueBase::_FLOAT);
   _argTypes[1].push_back(ValueBase::_FLOAT);
   _argTypes[2].push_back(ValueBase::_FLOAT);
   _argTypes[3].push_back(ValueBase::_FLOAT);
   _argTypes[4].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
Value FunSVGBox::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   SVGModule * module = (SVGModule *)(ModuleManager::instance()->getModule("SVG"));

   if (!module)
      return _objVal;

   ValueSVGPtr svg = SPTR_DCAST(ValueSVG, _objVal.ptr());
   module->box(svg, _vals[0].get_Float(), _vals[1].get_Float(), _vals[2].get_Float(), _vals[3].get_Float(), _vals.size() > 4 ? _vals[4].get_Float() : 0);

   return _objVal;
}
//=======================================================================================
FunSVGCircle::FunSVGCircle(ScriptPtr script)
{
   _name = "circle";
   _script = script;
   _num_calls = 0;

   _objRequired = true;
   _objTypes.push_back(ValueBase::_SVG);

   _minArgs = 3;
   _argTypes.resize(3);
   _argTypes[0].push_back(ValueBase::_FLOAT);
   _argTypes[1].push_back(ValueBase::_FLOAT);
   _argTypes[2].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
Value FunSVGCircle::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   SVGModule * module = (SVGModule *)(ModuleManager::instance()->getModule("SVG"));

   if (!module)
      return _objVal;

   ValueSVGPtr svg = SPTR_DCAST(ValueSVG, _objVal.ptr());
   module->circle(svg, _vals[0].get_Float(), _vals[1].get_Float(), _vals[2].get_Float());

   return _objVal;
}
//=======================================================================================
FunSVGCircleFill::FunSVGCircleFill(ScriptPtr script)
{
   _name = "circle_fill";
   _script = script;
   _num_calls = 0;

   _objRequired = true;
   _objTypes.push_back(ValueBase::_SVG);

   _minArgs = 3;
   _argTypes.resize(3);
   _argTypes[0].push_back(ValueBase::_FLOAT);
   _argTypes[1].push_back(ValueBase::_FLOAT);
   _argTypes[2].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
Value FunSVGCircleFill::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   SVGModule * module = (SVGModule *)(ModuleManager::instance()->getModule("SVG"));

   if (!module)
      return _objVal;

   ValueSVGPtr svg = SPTR_DCAST(ValueSVG, _objVal.ptr());
   module->circle_fill(svg, _vals[0].get_Float(), _vals[1].get_Float(), _vals[2].get_Float());

   return _objVal;
}
//=======================================================================================
FunSVGPoint::FunSVGPoint(ScriptPtr script)
{
   _name = "point";
   _script = script;
   _num_calls = 0;

   _objRequired = true;
   _objTypes.push_back(ValueBase::_SVG);

   _minArgs = 3;
   _argTypes.resize(3);
   _argTypes[0].push_back(ValueBase::_FLOAT);
   _argTypes[1].push_back(ValueBase::_FLOAT);
   _argTypes[2].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
Value FunSVGPoint::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   SVGModule * module = (SVGModule *)(ModuleManager::instance()->getModule("SVG"));

   if (!module)
      return _objVal;

   ValueSVGPtr svg = SPTR_DCAST(ValueSVG, _objVal.ptr());
   module->point(svg, _vals[0].get_Float(), _vals[1].get_Float(), _vals[2].get_Float());

   return _objVal;
}
//=======================================================================================
FunSVGArc::FunSVGArc(ScriptPtr script)
{
   _name = "arc";
   _script = script;
   _num_calls = 0;

   _objRequired = true;
   _objTypes.push_back(ValueBase::_SVG);

   _minArgs = 5;
   _argTypes.resize(5);
   _argTypes[0].push_back(ValueBase::_FLOAT);
   _argTypes[1].push_back(ValueBase::_FLOAT);
   _argTypes[2].push_back(ValueBase::_FLOAT);
   _argTypes[3].push_back(ValueBase::_FLOAT);
   _argTypes[4].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
Value FunSVGArc::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   SVGModule * module = (SVGModule *)(ModuleManager::instance()->getModule("SVG"));

   if (!module)
      return _objVal;

   ValueSVGPtr svg = SPTR_DCAST(ValueSVG, _objVal.ptr());
   module->arc(svg, _vals[0].get_Float(), _vals[1].get_Float(), _vals[2].get_Float(), _vals[3].get_Float(), _vals[4].get_Float());

   return _objVal;
}
//=======================================================================================
FunSVGText::FunSVGText(ScriptPtr script)
{
   _name = "text";
   _script = script;
   _num_calls = 0;

   _objRequired = true;
   _objTypes.push_back(ValueBase::_SVG);

   _minArgs = 3;
   _argTypes.resize(3);
   _argTypes[1].push_back(ValueBase::_FLOAT);
   _argTypes[2].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
Value FunSVGText::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   SVGModule * module = (SVGModule *)(ModuleManager::instance()->getModule("SVG"));

   if (!module)
      return _objVal;

   ValueSVGPtr svg = SPTR_DCAST(ValueSVG, _objVal.ptr());
   module->text(svg, _vals[0].get_Str(), _vals[1].get_Float(), _vals[2].get_Float());

   return _objVal;
}
//=======================================================================================
FunSVGPolygon::FunSVGPolygon(ScriptPtr script)
{
   _name = "polygon";
   _script = script;
   _num_calls = 0;

   _objRequired = true;
   _objTypes.push_back(ValueBase::_SVG);

   _minArgs = 2;
   _argTypes.resize(3);
   _argTypes[0].push_back(ValueBase::_ARRAY);
   _argTypes[2].push_back(ValueBase::_ARRAY);
}
//=======================================================================================
Value FunSVGPolygon::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   SVGModule * module = (SVGModule *)(ModuleManager::instance()->getModule("SVG"));

   if (!module)
      return _objVal;

   vector<Float> x;
   vector<Float> y;

   ValueBasePtr valueZero = make_shared<ValueFloat>(0);
   ValueBasePtr valueOne = make_shared<ValueFloat>(1);

   for (size_t i = 0; i < _vals[0].size(); i++)
   {     
      ValueBasePtr point = _vals[0][i].value();

      x.push_back(point->getArrVal(point, valueZero)->get_Float());
      y.push_back(point->getArrVal(point, valueOne)->get_Float());
   }

   bool fill = _vals[1].get_Logic().isTrue();

   vector<size_t> way;

   if (_vals.size() > 2)
   {
      for (size_t i = 0; i < _vals[2].size(); i++)
      {
         way.push_back(_vals[2][i]->get_UInt());
      }
   }

   ValueSVGPtr svg = SPTR_DCAST(ValueSVG, _objVal.ptr());
   module->polygon(svg, x, y, fill, way);

   return _objVal;
}
//=======================================================================================
FunSVGTransBeg::FunSVGTransBeg(ScriptPtr script)
{
   _name = "transf_beg";
   _script = script;
   _num_calls = 0;

   _objRequired = true;
   _objTypes.push_back(ValueBase::_SVG);

   _minArgs = 2;
}
//=======================================================================================
Value FunSVGTransBeg::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   SVGModule * module = (SVGModule *)(ModuleManager::instance()->getModule("SVG"));

   if (!module)
      return _objVal;

   Float dx = _vals[0].get_Float();
   Float dy = _vals[1].get_Float();
   Float phi = _vals.size() > 2 ? _vals[2].get_Float() : 0;
   Float sx = _vals.size() > 3 ? _vals[3].get_Float() : 1;
   Float sy = _vals.size() > 4 ? _vals[4].get_Float() : 1;

   ValueSVGPtr svg = SPTR_DCAST(ValueSVG, _objVal.ptr());
   module->transf_beg(svg, dx, dy, phi, sx, sy);

   return _objVal;
}
//=======================================================================================
FunSVGTransEnd::FunSVGTransEnd(ScriptPtr script)
{
   _name = "transf_end";
   _script = script;
   _num_calls = 0;

   _objRequired = true;
   _objTypes.push_back(ValueBase::_SVG);
}
//=======================================================================================
Value FunSVGTransEnd::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   SVGModule * module = (SVGModule *)(ModuleManager::instance()->getModule("SVG"));

   ValueSVGPtr svg = SPTR_DCAST(ValueSVG, _objVal.ptr());
   module->transf_end(svg);

   return _objVal;
}
//=======================================================================================
FunSVGTransAll::FunSVGTransAll(ScriptPtr script)
{
   _name = "transf_all";
   _script = script;
   _num_calls = 0;

   _objRequired = true;
   _objTypes.push_back(ValueBase::_SVG);

   _minArgs = 2;
}
//=======================================================================================
Value FunSVGTransAll::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   SVGModule * module = (SVGModule *)(ModuleManager::instance()->getModule("SVG"));

   if (!module)
      return _objVal;

   Float dx = _vals[0].get_Float();
   Float dy = _vals[1].get_Float();
   Float phi = _vals.size() > 2 ? _vals[2].get_Float() : 0;
   Float sx = _vals.size() > 3 ? _vals[3].get_Float() : 1;
   Float sy = _vals.size() > 4 ? _vals[4].get_Float() : 1;

   ValueSVGPtr svg = SPTR_DCAST(ValueSVG, _objVal.ptr());
   module->transf_all(svg, dx, dy, phi, sx, sy);

   return _objVal;
}
//=======================================================================================
FunSVGSave::FunSVGSave(ScriptPtr script)
{
   _name = "save";
   _script = script;
   _num_calls = 0;

   _objRequired = true;
   _objTypes.push_back(ValueBase::_SVG);

   _minArgs = 3;
   _argTypes.resize(3);
   _argTypes[1].push_back(ValueBase::_FLOAT);
   _argTypes[2].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
Value FunSVGSave::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   SVGModule * module = (SVGModule *)(ModuleManager::instance()->getModule("SVG"));

   if (!module)
      return _objVal;

   ValueSVGPtr svg = SPTR_DCAST(ValueSVG, _objVal.ptr());
   module->save(svg, _vals[0].get_Str(), _vals[1].get_Float(), _vals[2].get_Float());

   return _objVal;
}
//=======================================================================================