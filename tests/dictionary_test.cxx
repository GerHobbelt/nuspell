/* Copyright 2018 Sander van Geloven
 *
 * This file is part of Nuspell.
 *
 * Nuspell is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nuspell is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Nuspell.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "catch.hpp"

#include <iostream>

#include "../src/nuspell/dictionary.hxx"

using namespace std;
using namespace std::literals::string_literals;
using namespace nuspell;

TEST_CASE("parse", "[dictionary]")
{
	CHECK_THROWS_AS(Dictionary::load_from_aff_dic(""),
	                std::ios_base::failure);
	CHECK_THROWS_WITH(Dictionary::load_from_aff_dic(""),
	                  "aff file not found: iostream error");
}
TEST_CASE("simple", "[dictionary]")
{
	auto d = Dict_Base();
	d.set_encoding_and_language("UTF-8");

	auto words = {"table", "chair", "book", "fóóáár", "áárfóóĳ"};
	for (auto& x : words)
		d.words.insert({x, {}});

	auto good = {L"",      L".",    L"..",     L"table",
	             L"chair", L"book", L"fóóáár", L"áárfóóĳ"};
	for (auto& g : good)
		CHECK(d.spell_priv<wchar_t>(g) == true);

	auto wrong = {L"tabel", L"chiar",    L"boko", L"xyyz",  L"fooxy",
	              L"xyfoo", L"fooxybar", L"ééőő", L"fóóéé", L"őőáár"};
	for (auto& w : wrong)
		CHECK(d.spell_priv<wchar_t>(w) == false);
}

TEST_CASE("suffixes", "[dictionary]")
{
	auto d = Dict_Base();
	d.set_encoding_and_language("UTF-8");

	d.words.emplace("berry", u"T");
	d.words.emplace("May", u"T");
	d.words.emplace("vary", u"");

	d.wide_structures.suffixes.emplace(u'T', true, L"y", L"ies", Flag_Set(),
	                                   L".[^aeiou]y");

	auto good = {L"berry", L"Berry", L"berries", L"BERRIES",
	             L"May",   L"MAY",   L"vary"};
	for (auto& g : good)
		CHECK(d.spell_priv<wchar_t>(g) == true);

	auto wrong = {L"beRRies", L"Maies", L"MAIES", L"maies", L"varies"};
	for (auto& w : wrong)
		CHECK(d.spell_priv<wchar_t>(w) == false);
}

TEST_CASE("complex_prefixes", "[dictionary]")
{
	auto d = Dict_Base();
	d.set_encoding_and_language("UTF-8");

	d.words.emplace("drink", u"X");
	d.wide_structures.suffixes.emplace(u'Y', true, L"", L"s", Flag_Set(),
	                                   L".");
	d.wide_structures.suffixes.emplace(u'X', true, L"", L"able",
	                                   Flag_Set(u"Y"), L".");

	auto good = {L"drink", L"drinkable", L"drinkables"};
	for (auto& g : good)
		CHECK(d.spell_priv<wchar_t>(g) == true);

	auto wrong = {L"drinks"};
	for (auto& w : wrong)
		CHECK(d.spell_priv<wchar_t>(w) == false);
}

TEST_CASE("extra_stripping", "[dictionary]")
{
	auto d = Dict_Base();
	d.set_encoding_and_language("UTF-8");
	d.complex_prefixes = true;

	d.words.emplace("aa", u"ABC");
	d.words.emplace("bb", u"XYZ");

	d.wide_structures.prefixes.emplace(u'A', true, L"", L"W",
	                                   Flag_Set(u"B"), L"aa");
	d.wide_structures.prefixes.emplace(u'B', true, L"", L"Q",
	                                   Flag_Set(u"C"), L"Wa");
	d.wide_structures.suffixes.emplace(u'C', true, L"", L"E", Flag_Set(),
	                                   L"a");
	d.wide_structures.prefixes.emplace(u'X', true, L"b", L"1",
	                                   Flag_Set(u"Y"), L"b");
	d.wide_structures.suffixes.emplace(u'Y', true, L"", L"2",
	                                   Flag_Set(u"Z"), L"b");
	d.wide_structures.prefixes.emplace(u'Z', true, L"", L"3", Flag_Set(),
	                                   L"1");
	// complex strip suffix prefix prefix
	CHECK(d.spell_priv<wchar_t>(L"QWaaE") == true);
	// complex strip prefix suffix prefix
	CHECK(d.spell_priv<wchar_t>(L"31b2") == true);
}

TEST_CASE("break_pattern", "[dictionary]")
{
	auto d = Dict_Base();
	d.set_encoding_and_language("UTF-8");
	d.forbid_warn = true;
	d.warn_flag = *u"W";

	d.words.emplace("user", u"");
	d.words.emplace("interface", u"");
	d.words.emplace("interface-interface", u"W");

	d.wide_structures.break_table = {L"-", L"++++++$"};

	auto good = {L"user", L"interface", L"user-interface",
	             L"interface-user", L"user-user"};
	for (auto& g : good)
		CHECK(d.spell_priv<wchar_t>(g) == true);

	auto wrong = {L"user--interface", L"user interface",
	              L"user - interface", L"interface-interface"};
	for (auto& w : wrong)
		CHECK(d.spell_priv<wchar_t>(w) == false);
}

TEST_CASE("spell_casing_upper", "[dictionary]")
{
	auto d = Dict_Base();
	d.set_encoding_and_language("UTF-8");
	d.words.emplace("Sant'Elia", u"");
	d.words.emplace("d'Osormort", u"");

	auto good = {L"SANT'ELIA", L"D'OSORMORT"};
	for (auto& g : good)
		CHECK(d.spell_priv<wchar_t>(g) == true);
}

TEST_CASE("rep_suggest", "[dictionary]")
{
	auto d = Dict_Base();
	d.set_encoding_and_language("UTF-8");

	auto table = vector<pair<wstring, wstring>>();
	table.push_back(pair<wstring, wstring>(L"ph", L"f"));
	table.push_back(pair<wstring, wstring>(L"shun$", L"tion"));
	table.push_back(pair<wstring, wstring>(L"^voo", L"foo"));
	table.push_back(pair<wstring, wstring>(L"^alot$", L"a lot"));
	d.wide_structures.replacements = Replacement_Table<wchar_t>(table);

	auto good = L"fat";
	d.words.emplace("fat", u"");
	CHECK(d.spell_priv<wchar_t>(good) == true);
	auto w = wstring(L"phat");
	CHECK(d.spell_priv<wchar_t>(w) == false);
	auto out = List_Strings<wchar_t>();
	auto sug = List_Strings<wchar_t>();
	sug.push_back(good);
	d.rep_suggest(w, out);
	CHECK(out == sug);
	out.clear();
	sug.clear();
	auto g = wstring(good);
	d.rep_suggest(g, out);

	good = L"station";
	d.words.emplace("station", u"");
	CHECK(d.spell_priv<wchar_t>(good) == true);
	w = wstring(L"stashun");
	CHECK(d.spell_priv<wchar_t>(w) == false);
	out.clear();
	sug.clear();
	sug.push_back(good);
	d.rep_suggest(w, out);
	CHECK(out == sug);
	d.words.emplace("stations", u"");
	w = wstring(L"stashuns");
	CHECK(d.spell_priv<wchar_t>(w) == false);
	out.clear();
	sug.clear();
	d.rep_suggest(w, out);
	CHECK(out == sug);

	good = L"food";
	d.words.emplace("food", u"");
	CHECK(d.spell_priv<wchar_t>(good) == true);
	w = wstring(L"vood");
	CHECK(d.spell_priv<wchar_t>(w) == false);
	out.clear();
	sug.clear();
	sug.push_back(good);
	d.rep_suggest(w, out);
	CHECK(out == sug);
	w = wstring(L"vvood");
	CHECK(d.spell_priv<wchar_t>(w) == false);
	out.clear();
	sug.clear();
	d.rep_suggest(w, out);
	CHECK(out == sug);

	good = L"a lot";
	d.words.emplace("a lot", u"");
	CHECK(d.spell_priv<wchar_t>(good) == true);
	w = wstring(L"alot");
	CHECK(d.spell_priv<wchar_t>(w) == false);
	out.clear();
	sug.clear();
	sug.push_back(good);
	d.rep_suggest(w, out);
	CHECK(out == sug);
	w = wstring(L"aalot");
	CHECK(d.spell_priv<wchar_t>(w) == false);
	out.clear();
	sug.clear();
	d.rep_suggest(w, out);
	CHECK(out == sug);
	w = wstring(L"alott");
	CHECK(d.spell_priv<wchar_t>(w) == false);
	out.clear();
	sug.clear();
	d.rep_suggest(w, out);
	CHECK(out == sug);
}

TEST_CASE("extra_char_suggest", "[dictionary]")
{
	auto d = Dict_Base();
	d.set_encoding_and_language("UTF-8");

	auto good = L"abcd";
	d.wide_structures.try_chars = good;
	d.words.emplace("abcd", u"");
	CHECK(d.spell_priv<wchar_t>(good) == true);

	auto w = wstring(L"abxcd");
	CHECK(d.spell_priv<wchar_t>(w) == false);

	auto out = List_Strings<wchar_t>();
	auto sug = List_Strings<wchar_t>();
	sug.push_back(good);
	d.extra_char_suggest(w, out);
	CHECK(out == sug);
}

TEST_CASE("map_suggest", "[dictionary]")
{
	auto d = Dict_Base();
	d.set_encoding_and_language("UTF-8");

	auto good = L"äbcd";
	d.words.emplace("äbcd", u"");
	d.wide_structures.similarities.push_back(
	    Similarity_Group<wchar_t>(L"aäâ"));
	CHECK(d.spell_priv<wchar_t>(good) == true);

	auto w = wstring(L"abcd");
	CHECK(d.spell_priv<wchar_t>(w) == false);

	auto out = List_Strings<wchar_t>();
	auto sug = List_Strings<wchar_t>();
	sug.push_back(good);
	d.map_suggest(w, out);
	CHECK(out == sug);

	d.words.emplace("æon", u"");
	d.wide_structures.similarities.push_back(
	    Similarity_Group<wchar_t>(L"æ(ae)"));
	good = L"æon";
	CHECK(d.spell_priv<wchar_t>(good) == true);
	w = wstring(L"aeon");
	CHECK(d.spell_priv<wchar_t>(w) == false);
	out.clear();
	sug.clear();
	sug.push_back(good);
	d.map_suggest(w, out);
	CHECK(out == sug);

	d.words.emplace("zijn", u"");
	d.wide_structures.similarities.push_back(
	    Similarity_Group<wchar_t>(L"(ij)ĳ"));
	good = L"zijn";
	CHECK(d.spell_priv<wchar_t>(good) == true);
	w = wstring(L"zĳn");
	CHECK(d.spell_priv<wchar_t>(w) == false);
	out.clear();
	sug.clear();
	sug.push_back(good);
	d.map_suggest(w, out);
	CHECK(out == sug);
}

TEST_CASE("keyboard_suggest", "[dictionary]")
{
	auto d = Dict_Base();
	d.set_encoding_and_language("UTF-8");

	auto good1 = L"abcd";
	auto good2 = L"Abb";
	d.words.emplace("abcd", u"");
	d.words.emplace("Abb", u"");
	d.wide_structures.keyboard_closeness = L"uiop|xdf|nm";
	CHECK(d.spell_priv<wchar_t>(good1) == true);

	auto w = wstring(L"abcf");
	CHECK(d.spell_priv<wchar_t>(w) == false);

	auto out = List_Strings<wchar_t>();
	auto sug = List_Strings<wchar_t>();
	sug.push_back(good1);
	d.keyboard_suggest(w, out);
	CHECK(out == sug);

	w = wstring(L"abcx");
	CHECK(d.spell_priv<wchar_t>(w) == false);

	out.clear();
	sug.clear();
	sug.push_back(good1);
	d.keyboard_suggest(w, out);
	CHECK(out == sug);

	w = wstring(L"abcg");
	CHECK(d.spell_priv<wchar_t>(w) == false);

	out.clear();
	sug.clear();
	d.keyboard_suggest(w, out);
	CHECK(out == sug);

	w = wstring(L"abb");
	CHECK(d.spell_priv<wchar_t>(w) == false);

	out.clear();
	sug.clear();
	sug.push_back(good2);
	d.keyboard_suggest(w, out);
	CHECK(out == sug);
}

TEST_CASE("bad_char_suggest", "[dictionary]")
{
	auto d = Dict_Base();
	d.set_encoding_and_language("UTF-8");

	auto good = L"abcd";
	d.words.emplace("abcd", u"");
	d.wide_structures.try_chars = good;
	CHECK(d.spell_priv<wchar_t>(good) == true);

	auto w = wstring(L"abce");
	CHECK(d.spell_priv<wchar_t>(w) == false);

	auto out = List_Strings<wchar_t>();
	auto sug = List_Strings<wchar_t>();
	sug.push_back(good);
	d.bad_char_suggest(w, out);
	CHECK(out == sug);
}

TEST_CASE("forgotten_char_suggest", "[dictionary]")
{
	auto d = Dict_Base();
	d.set_encoding_and_language("UTF-8");

	auto good = L"abcd";
	d.words.emplace("abcd", u"");
	d.wide_structures.try_chars = good;
	CHECK(d.spell_priv<wchar_t>(good) == true);

	auto w = wstring(L"abd");
	CHECK(d.spell_priv<wchar_t>(w) == false);

	auto out = List_Strings<wchar_t>();
	auto sug = List_Strings<wchar_t>();
	sug.push_back(good);
	d.forgotten_char_suggest(w, out);
	CHECK(out == sug);
}

#if 0
TEST_CASE("phonetic_suggest", "[dictionary]") {}
#endif

#if 0
TEST_CASE("long word", "[dictionary]")
{
	auto d = Dict_Base();
	d.set_encoding_and_language("UTF-8");

	// 18 times abcdefghij (10 characters) = 180 characters
	auto good =
	    L"abcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcde"
	    L"fghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghij"
	    L"abcdefghijabcdefghijabcdefghijabcdefghijabcdefghij";
	// 18 times abcdefghij (10 characters) + THISISEXTRA = 191 characters
	auto toolong =
	    L"abcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcde"
	    L"fghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghij"
	    L"abcdefghijabcdefghijabcdefghijabcdefghijabcdefghijTHISISEXTRA";
	// 18 times abcdefghij (10 characters) = 180 characters
	d.words.emplace(
	    "abcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdef"
	    "ghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijab"
	    "cdefghijabcdefghijabcdefghijabcdefghijabcdefghij",
	    u"");
	CHECK(d.spell(good) == true);
	CHECK(d.spell(toolong) == true);

	auto out = List_Strings<wchar_t>();
	auto sug = List_Strings<wchar_t>();
	d.suggest(toolong, out);
	CHECK(out == sug);
}
#endif
