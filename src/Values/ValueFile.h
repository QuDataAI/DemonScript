/*!                           Вещественная переменная


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ValueFileH
#define ValueFileH

#include "ValueBase.h"

SPTR_DEF(ValueFile);

//=======================================================================================
//! Вещественный тип
// 
class ValueFile : public ValueBase
{
   string  _name;
   Int     _fileId;
   UInt    _size;
public:
   ValueFile(string name, int fileId) : _name(name), _fileId(fileId)  { }
   /*!
      Вывести в поток out текущее значение
      \param поток в который выводим значение
      \return результирующий поток
   */
   ostream& print(ostream& out) { return out << _name; }
   /*!
      Тип значения в строковом виде
      \return тип значения в строковом виде
   */
   string   type_str() { return "file"; }
   /*!
      Тип значения в числовом виде
      \return тип значения в числовом виде
   */
   Kind     type() { return _FILE; }

   shared_ptr<ValueBase> copy() const { return shared_ptr<ValueFile>(new ValueFile(_name, _fileId)); }
   /*!
      Конвертировать значение в строку
      \return строковое значение
   */
   string toString();
   /*!
   Получить целочисленное знаковое значение
   \return целочисленное знаковое значение
   */
   virtual Int     get_Int() { return _fileId; }
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
      if (val->type() == ValueBase::_FILE)
      {
         *this = *(ValueFile *)val.get();
      }
   };
};

#endif 