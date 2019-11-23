#include <iostream>
#include <cmath>
#include "JsonWriter.hpp"
using namespace std;
using namespace jw;
int main()
{
auto writer = new JsonWriter;
writer->startArray();

  writer->startShortObject ();
    writer->KeyValue ("name", "shiyanlou1");
    writer->KeyValue ("age", 3);
  writer->endObject ();

  writer->startObject ();
    writer->KeyValue ("skills", "c++");
    writer->KeyValue ("skills","python");
    writer->KeyValue ("skills","php");
    writer->KeyValue ("skills","java");
    writer->KeyValue ("url", "http://shiyanlou.com");

    writer->Key ("path");
    writer->startArray ();
      writer->Value ("web");
      writer->Value ("algorithm");
      writer->Value ("linux");
    writer->endArray ();

    writer->Key ("short-array");
    writer->startShortArray ();
      writer->Value (1);
      writer->Value ((uint64_t)0xabcdef123456);
      writer->Value (M_PI);
    writer->endContainer ();

  writer->endObject (),
  writer->Value (false);
writer->endArray ();
}