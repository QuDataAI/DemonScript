/*!                           Вещественная переменная


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ValueSVGH
#define ValueSVGH

#include "ValueBase.h"

SPTR_DEF(ValueSVG);

//=======================================================================================
//! Вещественный тип
// 
class ValueSVG : public ValueBase
{
   friend class SVGModule;

   string  _val;
   string  _colorFill = "black";
   string  _colorLine = "black";
   string  _colorText = "black";
   Float   _widhtLine = 1;
   Float   _sizeFont = 16;
public:
   ValueSVG(string val) : _val(val) { }
   /*!
      Вывести в поток out текущее значение
      \param поток в который выводим значение
      \return результирующий поток
   */
   ostream& print(ostream& out) { return out << _val; }
   /*!
      Тип значения в строковом виде
      \return тип значения в строковом виде
   */
   string   type_str() { return "svg"; }
   /*!
      Тип значения в числовом виде
      \return тип значения в числовом виде
   */
   Kind     type() { return _SVG; }
   /*!
   Тип присваивания значения
   \return присваивания значения
   */
   SetType  setType() const { return ST_REFERENCE; }

   shared_ptr<ValueBase> copy() const { return shared_ptr<ValueSVG>(new ValueSVG(_val)); }
   /*!
      Конвертировать значение в строку
      \return строковое значение
   */
   string toString();
   /*!
   Очистить значения (для массива)
   */
   virtual void clear() { _val = "";  }
   /*!
   Получить целочисленное знаковое значение
   \return целочисленное знаковое значение
   */
   virtual Int     get_Int() { return 0; }
   /*!
   Размерность данных
   \return размерность данных
   */
   virtual UInt size();
   /*!
   Присвоить переменной Value
   \param val значение
   */
   virtual void operator=(const shared_ptr<ValueBase> &val) 
   { 
      if (val->type() == ValueBase::_SVG)
      {
         *this = *(ValueSVG *)val.get();
      }
   };
};

#endif 