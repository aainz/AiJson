# AiJson


## Table of Contents
- [About](#about)
- [Installation](#installation)
- [Examples](#examples) 
- [API](#api)  
 - [Structs](#structs)
 - [Functions](#functions)
- [License](#license)

## About

AiJson is a small, fast and portable recursive descent JSON parser written in C89.

### Features

 - Ignores UTF-8 BOM.
 - Portable, only uses the C standard library.
 - Can convert a JSON object to a string and format it if needed.
 - Keeps track of the line and column of errors.
 - Can be used for serialization.
 - Easy to insert elements because JSON objects and arrays are represented as linked lists.
 - Simple API.

## Installation

Simply add the source files to your project and include *AiJson.h*.

## Examples

### Iterating

#### JSON array

```C
AiListNode *curr = array->first;
while(curr)
{
   AiJsonValue *value = (AiJsonValue *) curr->data;
   if(value->type == AI_JSON_SLONG)
      printf("%ld\n", value->data.sLong); 
   curr = curr->next;
}
```
    
#### JSON object

```C
AiListNode *curr = object->first;
while(curr)
{
   AiJsonObjectField *field = (AiJsonObjectField *) curr->data;
   if(field->value.type == AI_JSON_SLONG)
      printf("%s: %ld\n", field->name, field->value.data.sLong);
   curr = curr->next;
}
```
    
---

### Serialization

```C
typedef struct Person Person;

struct Person
{
   char  *firstName;
   int    age;
   int    isMale;
   int    isProgrammer;
   int    friendsAmount;
   char **friendsFirstName;
};

char *Person2json(Person *person)
{
   AiJson *json = AiJsonParse("{}"); //creates empty object
   char   *str = 0;
   int     i;
   AiJsonValue value;
   if(!json || json->error != AI_JSON_E_OK || !person)
      goto exit;

   value.type        = AI_JSON_STRING;
   value.data.string = person->firstName;
   if(!AiJsonInsertValueObject(json, json->root.data.object, "firstName", &value))
      goto exit;

   value.type        = AI_JSON_SLONG;
   value.data.sLong  = person->age;
   if(!AiJsonInsertValueObject(json, json->root.data.object, "age", &value))
      goto exit;

   value.type        = AI_JSON_BOOL;
   value.data.sLong  = person->isMale;
   if(!AiJsonInsertValueObject(json, json->root.data.object, "isMale", &value))
      goto exit;

   value.data.sLong  = person->isProgrammer;
   if(!AiJsonInsertValueObject(json, json->root.data.object, "isProgrammer", &value))
      goto exit;

   value.type       = AI_JSON_ARRAY;
   value.data.array = AiListCreate();
   if(!value.data.array)
      goto exit;
   for(i = 0; i < person->friendsAmount; ++i)
   {
      AiJsonValue strValue;
      strValue.type = AI_JSON_STRING;
      strValue.data.string = person->friendsFirstName[i];
      if(!AiJsonInsertValueArray(json, value.data.array, &strValue))
         goto exit;
   }
   if(!AiJsonInsertValueObject(json, json->root.data.object, "friendsFirstName", &value))
      goto exit;

   str = AiJsonDump(json, 0);
exit:
   AiJsonDestroy(json);
   return str;
}

AiJson *Json2person(char *str, Person *person)
{
   AiJson *json = AiJsonParse(str);
   int     i;
   AiJsonValue *value;
   AiListNode  *curr;

   if(!json || !person)
      goto error;

   if(!(value = AiJsonGetValueObject(json->root.data.object, "firstName")) || value->type != AI_JSON_STRING)
      goto error;
   person->firstName = value->data.string;

   if(!(value = AiJsonGetValueObject(json->root.data.object, "age")) || value->type != AI_JSON_SLONG)
      goto error;
   person->age = value->data.sLong;

   if(!(value = AiJsonGetValueObject(json->root.data.object, "isMale")) || value->type != AI_JSON_BOOL)
      goto error;
   person->isMale = value->data.sLong;

   if(!(value = AiJsonGetValueObject(json->root.data.object, "isProgrammer")) || value->type != AI_JSON_BOOL)
      goto error;
   person->isProgrammer = value->data.sLong;

   if(!(value = AiJsonGetValueObject(json->root.data.object, "friendsFirstName")) || value->type != AI_JSON_ARRAY)
      goto error;
   person->friendsAmount = value->data.array->len;
   person->friendsFirstName = (char **) malloc(person->friendsAmount);
   if(!person->friendsFirstName)
      goto error;
   curr = value->data.array->first;
   for(i = 0; curr; ++i)
   {
      AiJsonValue *value = (AiJsonValue *) curr->data;
      if(value->type != AI_JSON_STRING)
         goto error; 
      person->friendsFirstName[i] = value->data.string;
      curr = curr->next;
   }

   return json;
error:
   free(person->friendsFirstName);
   AiJsonDestroy(json);
   return 0;
}
```

## API

## Structs

### AiJsonValue

#### Members

- *type:* *AiJsonType* enum.
 - *AI_JSON_STRING*
 - *AI_JSON_DOUBLE*
 - *AI_JSON_ULONG*
 - *AI_JSON_SLONG*
 - *AI_JSON_OBJECT*
 - *AI_JSON_ARRAY*
 - *AI_JSON_BOOL*
 - *AI_JSON_NULL*
- *data:* union.
 - *string:* C string.
 - *sLong:* Signed 32 bit integer.
 - *uLong:* Unsigned 32 bit integer.
 - *dbl:* double.
 - *object:* Pointer to *AiList*.
 - *array:* Pointer to *AiList*.
 - *boolean:* -

---

### AiJsonObjectField

*AiList* contains pointers to this struct when the list represents a JSON object.

#### Members

- *name:* C string
- *value:* *AiJsonValue*

---

### AiJson

#### Members

- *root:* Pointer to *AiJsonValue* representing the root object or array.
- *error:* *AiJsonError* enum.
 - *AI_JSON_E_OK*
 - *AI_JSON_E_UNEXPECTED_SYMBOL*
 - *AI_JSON_E_UNEXPECTED_EOF*
 - *AI_JSON_E_INVALID_HEX_DIGIT*
 - *AI_JSON_E_INVALID_ESCAPE_SEQUENCE*
 - *AI_JSON_E_CHAR_MUST_BE_ESCAPED*
 - *AI_JSON_E_INVALID_NUM*
 - *AI_JSON_E_EXPECTED_ARRAY_CLOSE*
 - *AI_JSON_E_EXPECTED_NAME*
 - *AI_JSON_E_EXPECTED_NAME_SEPARATOR*
 - *AI_JSON_E_EXPECTED_OBJECT_CLOSE*
 - *AI_JSON_E_UNKNOWN*
 - *AI_JSON_E_ALLOC*
 - *AI_JSON_E_INVALID_VALUE*
- *errorMsg:* C string.
- *line:* Line number of an error.
- *column:* Column number of an error.

---

### AiList

#### Members

- *first:* First node.
- *last:* Last node.
- *len:* List length.

---

### AiListNode

#### Members

- *prev:* Previous node.
- *next:* Next node.
- *data:* Pointer to void.

## Functions

### AiJsonParse

`AiJson *AiJsonParse(char *str);`

Parses JSON.
    
#### Parameters:

- *str:* C string.

#### Return Value

Returns a *null pointer* if the function fails to allocate memory or if *str* is a *null pointer*, otherwise it returns a pointer to *AiJson*. Check if the *error* member of the struct is *AI_JSON_E_OK* before calling further functions.

---

### AiJsonDestroy

`void AiJsonDestroy(AiJson *json);`

#### Parameters:

- *json:* *AiJson* pointer returned by *AiJsonParse*.

#### Return Value

None.

---

### AiJsonDump

`char *AiJsonDump(AiJson *json, int spaces);`

Creates a C string from an *AiJson* pointer returned by *AiJsonParse*.

#### Parameters

- *json:* Pointer to *AiJson* returned by *AiJsonParse*.
- *spaces:* Number of spaces for indentation, if *0* the returned string will contain no whitespaces, if more then *4* then it will be changed to that number.

### Return value

Returns a *null pointer* if the function fails to allocate memory otherwise returns a C string that must be deallocated with *free*.

---

### AiJsonGetValueObject

`AiJsonValue *AiJsonGetValueObject(AiList *object, char *name);`

Gets a value from a field in a JSON object with it's name.

#### Parameters

- *object:* *AiList* pointer.
- *name:* C string.

#### Return value

Returns a *null pointer* if *name* can't be found otherwise returns a pointer to *AiJsonValue*.


### AiJsonRemoveValueObject

`void AiJsonRemoveValueObject(AiList *object, char *name);`

Removes a field in a JSON object.

#### Parameters

- *object:* *AiList* pointer.
- *name:* C string.

#### Return value

None.

---

### AiJsonInsertValueObject

`int AiJsonInsertValueObject(AiJson *json, AiList *object, char *name, AiJsonValue *value);`

Inserts a field into a JSON object.

#### Parameters

- *json:* Pointer to *AiJson* returned by *AiJsonParse*.
- *object:* *AiList* pointer to insert into.
- *name:* C string.
- *value:* *AiJsonValue* pointer.

#### Return value

Returns *false* if the parameters are invalid or if the function fails to allocate memory otherwise returns *true*.

---

### AiJsonGetValueArray

`AiJsonValue *AiJsonGetValueArray(AiList *array, size_t index);`

Gets an *AiJsonValue* from a JSON array with an index. Do not iterate with the help of this function it is slow.


#### Parameters

- *array:* *AiList* pointer.
- *index:* index starting from 0.

#### Return value

Returns a *null pointer* if *index* doesn't exist otherwise returns a pointer to *AiJsonValue*.

### AiJsonRemoveValueArray

`void AiJsonRemoveValueArray(AiList *array, size_t index);`

Removes an index from a JSON array.

#### Parameters

- *array:* *AiList* pointer.
- *index:* index starting from 0.

#### Return value

None.

---

### AiJsonInsertValueArray

`int AiJsonInsertValueArray(AiJson *json, AiList *array, AiJsonValue *value)`

Inserts a value into a JSON array simple wrapper for *AiListInsert*.

#### Parameters

- *json:* Pointer to *AiJson* returned by *AiJsonParse*.
- *array:* *AiList* pointer.
- *value:* *AiJsonValue* pointer.

#### Return value

Returns *false* if the parameters are invalid or if the function fails to allocate memory otherwise returns *true*.

---

### AiListCreate

`AiList *AiListCreate();`

Creates a list to use as a JSON object or array or anything else.

#### Parameters

None.

#### Return value

Returns a *null pointer* if the function failed to allocate memory otherwise returns a pointer to *AiList*.

---

### AiListInsert

`int AiListInsert(AiList *list, void *data);`

#### Parameters

- *list:* Pointer to *AiList*.
- *data:* Pointer to void.

#### Return value

Returns *false* if the parameters are invalid or if the function fails to allocate memory otherwise returns *true*.

---

### AiListRemove

`void AiListRemove(AiList *list, AiListNode *node);`

#### Parameters

- *list:* Pointer to *AiList*.
- *node:* Pointer to *AiListNode*.

#### Return value

None.

---

### AiListDestroy

`void AiListDestroy(AiList *list);`

#### Parameters

- *list:* Pointer to *AiList*.

#### Return value

None.

## License

```
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
```