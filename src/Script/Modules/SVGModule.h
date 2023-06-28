/*!            Math - модуль математических функций используемых скриптом

Доступ к функциям модуля осуществляется через вызов Math.<имя функции>(<аргументы>)

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef SVGModuleH
#define SVGModuleH

#include "Module.h"
#include "ModuleFunction.h"
#include <random>

//=======================================================================================
//! Модуль математических функций
// 
class SVGModule : public Module
{
   INTERFACE_MODULE(SVGModule);
public:
   /*!
   Конверсия Float в строку с отбрасыванием лишних 0 после запятой и округлением
   \param v число для конерсии
   \return результирующая строка
   */
   inline string to_stringI(Float v) { return to_string((int)round(v)); }

   /*!
   Вернуть текущий цвет линии для заданного svg
   \param svg svg изображение с которым работаем
   \return цвет линии
   */
   string color_line(ValueSVGPtr svg);
   /*!
   Установить цвет линии для заданного svg   
   \param svg svg изображение с которым работаем
   \param val цвет для установки
   */
   void color_line(ValueSVGPtr svg, string val);
   /*!
   Вернуть текущий цвет заливки для заданного svg
   \param svg svg изображение с которым работаем
   \return цвет заливки
   */
   string color_fill(ValueSVGPtr svg);
   /*!
   Установить цвет заливки для заданного svg
   \param svg svg изображение с которым работаем
   \param val цвет для установки
   */
   void color_fill(ValueSVGPtr svg, string val);
   /*!
   Вернуть текущий цвет текста для заданного svg
   \param svg svg изображение с которым работаем
   \return цвет текста
   */
   string color_text(ValueSVGPtr svg);
   /*!
   Установить цвет текста для заданного svg
   \param svg svg изображение с которым работаем
   \param val цвет для установки
   */
   void color_text(ValueSVGPtr svg, string val);
   /*!
   Вернуть текущую толщинц линии для заданного svg
   \param svg svg изображение с которым работаем
   \return толщина линии
   */
   Float width_line(ValueSVGPtr svg);
   /*!
   Установить толщину линии для заданного svg
   \param svg svg изображение с которым работаем
   \param val значение для установки
   */
   void width_line(ValueSVGPtr svg, Float val);
   /*!
   Вернуть текущий размер шрифта для заданного svg
   \param svg svg изображение с которым работаем
   \return размер шрифта
   */
   Float size_font(ValueSVGPtr svg);
   /*!
   Установить размер шрифта для заданного svg
   \param svg svg изображение с которым работаем
   \param val размер шрифта
   */
   void size_font(ValueSVGPtr svg, Float val);   
   /*!
   Нарисовать прямую линию текущего цвета и толщины
   \param svg svg изображение с которым работаем
   \param x1 x-координата начала линии
   \param y1 y-координата начала линии
   \param x2 x-координата конца линии
   \param y2 y-координата конца линии
   */
   void line(ValueSVGPtr svg, Float x1, Float y1, Float x2, Float y2);
   /*!
   Нарисовать прямоугольник текущего цвета и толщины
   \param svg svg изображение с которым работаем
   \param x x-координата левого верхнего угла
   \param y y-координата левого верхнего угла
   \param w ширина
   \param h высота
   \param r радиус скругления углов
   */
   void rect(ValueSVGPtr svg, Float x, Float y, Float w, Float h, Float r);
   /*!
   Нарисовать залитый прямоугольник текущими цветами
   \param svg svg изображение с которым работаем
   \param x x-координата левого верхнего угла
   \param y y-координата левого верхнего угла
   \param w ширина
   \param h высота
   \param r радиус скругления углов
   */
   void box(ValueSVGPtr svg, Float x, Float y, Float w, Float h, Float r);
   /*!
   Нарисовать окружность текущего цвета и толщины
   \param svg svg изображение с которым работаем
   \param x x-координата центра
   \param y y-координата центра
   \param r радиус
   */
   void circle(ValueSVGPtr svg, Float x, Float y, Float r);
   /*!
   Нарисовать залитую окружность текущими цветами
   \param svg svg изображение с которым работаем
   \param x x-координата центра
   \param y y-координата центра
   \param r радиус
   */
   void circle_fill(ValueSVGPtr svg, Float x, Float y, Float r);
   /*!
   Нарисовать залитую окружность текущим цветам заливки
   \param svg svg изображение с которым работаем
   \param x x-координата центра
   \param y y-координата центра
   \param r радиус
   */
   void point(ValueSVGPtr svg, Float x, Float y, Float r);
   /*!
   Нарисовать дугу из центра x,y радиуса r от угла a1 до угла a2 против часовой стрелке.
   \param svg svg изображение с которым работаем
   \param x x-координата центра
   \param y y-координата центра
   \param r радиус
   \param a1 радиус
   \param a2 радиус
   */
   void arc(ValueSVGPtr svg, Float x, Float y, Float r, Float a1, Float a2);
   /*!
   Нарисовать текст txt (строка) в точке x,y. Будет центрироваться вокруг этой точки.
   \param svg svg изображение с которым работаем
   \param txt строка
   \param x x-координата центра
   \param y y-координата центра
   */
   void text(ValueSVGPtr svg, string txt, Float x, Float y);
   /*!
   Нарисовать замкнутый полигон по точкам в массиве pnts =[ [x,y], ...]
   \param svg svg изображение с которым работаем
   \param x x-координаты точек
   \param y y-координаты точек
   \param fill нужна ли заливка
   \param way если не пуст, то считается индексами в массиве x, y
   */
   void polygon(ValueSVGPtr svg, vector<Float> &x, vector<Float> &y, bool fill, vector<size_t> &way);
   /*!
   Сохранить svg в файл
   \param svg svg изображение с которым работаем
   \param fname имя файла
   \param w ширина канваса
   \param h высота канваса
   */
   void save(ValueSVGPtr svg, string fname, Float w, Float h);
   /*!
   Начать в svg-файле трансформацию сдвига dx, dy, вращения на угол phi и скейла sx, sy
   \param svg svg изображение с которым работаем
   \param dx сдвиг по x
   \param dy сдвиг по y
   \param phi угол поворота
   \param sx масштаб по x
   \param sy масштаб по y
   */
   void transf_beg(ValueSVGPtr svg, Float dx, Float dy, Float phi, Float sx, Float sy);
   /*!
   Завершить трансформацию
   \param svg svg изображение с которым работаем
   */
   void transf_end(ValueSVGPtr svg);
   /*!
   Применить ко всему, сформированному до сих пор svg-файлу сдвиг dx, dy, вращения на угол phi и скейл sx, sy
   \param svg svg изображение с которым работаем
   \param dx сдвиг по x
   \param dy сдвиг по y
   \param phi угол поворота
   \param sx масштаб по x
   \param sy масштаб по y
   */
   void transf_all(ValueSVGPtr svg, Float dx, Float dy, Float phi, Float sx, Float sy);
private:
};

//=======================================================================================
//!  
// 
class SVGFuncCreate : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   SVGFuncCreate();
};
////=======================================================================================
////!  
//// 
//class SVGFuncSave : public ModuleFunction
//{
//   INTERFACE_MODULE_FUNCTION;
//   SVGFuncSave();
//};
////=======================================================================================
////!  
//// 
//class SVGFuncClear : public ModuleFunction
//{
//   INTERFACE_MODULE_FUNCTION;
//   SVGFuncClear();
//};
////=======================================================================================
////!  
//// 
//class SVGFuncColorFill : public ModuleFunction
//{
//   INTERFACE_MODULE_FUNCTION;
//   SVGFuncColorFill();
//};
////=======================================================================================
////!  
//// 
//class SVGFuncColorLine : public ModuleFunction
//{
//   INTERFACE_MODULE_FUNCTION;
//   SVGFuncColorLine();
//};
////=======================================================================================
////!  
//// 
//class SVGFuncColorText : public ModuleFunction
//{
//   INTERFACE_MODULE_FUNCTION;
//   SVGFuncColorText();
//};
////=======================================================================================
////!  
//// 
//class SVGFuncWidthLine : public ModuleFunction
//{
//   INTERFACE_MODULE_FUNCTION;
//   SVGFuncWidthLine();
//};
////=======================================================================================
////!  
//// 
//class SVGFuncSizeFont : public ModuleFunction
//{
//   INTERFACE_MODULE_FUNCTION;
//   SVGFuncSizeFont();
//};
////=======================================================================================
////!  
//// 
//class SVGFuncLine : public ModuleFunction
//{
//   INTERFACE_MODULE_FUNCTION;
//   SVGFuncLine();
//};
////=======================================================================================
////!  
//// 
//class SVGFuncRect : public ModuleFunction
//{
//   INTERFACE_MODULE_FUNCTION;
//   SVGFuncRect();
//};
////=======================================================================================
////!  
//// 
//class SVGFuncBox : public ModuleFunction
//{
//   INTERFACE_MODULE_FUNCTION;
//   SVGFuncBox();
//};
////=======================================================================================
////!  
//// 
//class SVGFuncCircle : public ModuleFunction
//{
//   INTERFACE_MODULE_FUNCTION;
//   SVGFuncCircle();
//};
////=======================================================================================
////!  
//// 
//class SVGFuncCircleFill : public ModuleFunction
//{
//   INTERFACE_MODULE_FUNCTION;
//   SVGFuncCircleFill();
//};
////=======================================================================================
////!  
//// 
//class SVGFuncPoint : public ModuleFunction
//{
//   INTERFACE_MODULE_FUNCTION;
//   SVGFuncPoint();
//};
////=======================================================================================
////!  
//// 
//class SVGFuncArc : public ModuleFunction
//{
//   INTERFACE_MODULE_FUNCTION;
//   SVGFuncArc();
//};
////=======================================================================================
////!  
//// 
//class SVGFuncText : public ModuleFunction
//{
//   INTERFACE_MODULE_FUNCTION;
//   SVGFuncText();
//};
//=======================================================================================
//===ФУНКЦИИ НЕ ИЗ МОДУЛЯ, А ВЫЗЫВАЕМЫЕ ДЛЯ ОБЪЕКТА======================================
//===var S = SVG.create(); S.some_func(); ===============================================
//=======================================================================================
//! Установить цвет линии
//
class FunSVGColorLine : public Function
{
public:
   FunSVGColorLine(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Установить цвет заливки
//
class FunSVGColorFill : public Function
{
public:
   FunSVGColorFill(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Установить цвет текста
//
class FunSVGColorText : public Function
{
public:
   FunSVGColorText(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Установить толщину линии
//
class FunSVGWidthLine : public Function
{
public:
   FunSVGWidthLine(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Установить размер шрифта
//
class FunSVGSizeFont : public Function
{
public:
   FunSVGSizeFont(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Нарисовать линию
//
class FunSVGLine : public Function
{
public:
   FunSVGLine(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Нарисовать прямоугольник
//
class FunSVGRect : public Function
{
public:
   FunSVGRect(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Нарисовать залитый прямоугольник
//
class FunSVGBox : public Function
{
public:
   FunSVGBox(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Нарисовать окружность
//
class FunSVGCircle : public Function
{
public:
   FunSVGCircle(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Нарисовать закрашенную окружность
//
class FunSVGCircleFill : public Function
{
public:
   FunSVGCircleFill(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Нарисовать "толстую" точку
//
class FunSVGPoint : public Function
{
public:
   FunSVGPoint(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Нарисовать дугу из центра x,y радиуса r от угла a1 до угла a2 против часовой стрелке.
//
class FunSVGArc : public Function
{
public:
   FunSVGArc(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Нарисовать текст txt (строка) в точке x,y. Будет центрироваться вокруг этой точки.
//
class FunSVGText : public Function
{
public:
   FunSVGText(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Нарисовать замкнутый полигон по точкам в массиве pnts =[ [x,y], ...] Если fill = true - полигон залит цветом cFill Если определён массив номеров way, то точки из pnts беруться в последовательности way
//
class FunSVGPolygon : public Function
{
public:
   FunSVGPolygon(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Начать в svg-файле трансформацию сдвига dx, dy, *вращения на угол phi и скейла sx, sy(скейла и вращения может не быть)
//
class FunSVGTransBeg : public Function
{
public:
   FunSVGTransBeg(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Закончить трансформацию
//
class FunSVGTransEnd : public Function
{
public:
   FunSVGTransEnd(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Применить ко всему, сформированному до сих пор svg-файлу сдвиг dx, dy, 
//
class FunSVGTransAll : public Function
{
public:
   FunSVGTransAll(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Сохранить картинку в файл с прямоугольником канваса w,h
//
class FunSVGSave : public Function
{
public:
   FunSVGSave(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};


#endif
