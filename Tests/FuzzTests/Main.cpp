/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include <assert.h>
#include <random>
#include <thread>
#include <list>
#include <inttypes.h>
#include <iostream>

#include "../../Source/Jinx.h"
#include "clara.hpp"

using namespace Jinx;
using namespace clara;


static const char * s_testScripts[] =
{
	
	u8R"(

		-- Single line comment

		--- Multiline 
			comment ---
				
		---------
		Alternate
		multiline
		comment
		---------

		set a --- some comment --- to 123
   
	)",

	u8R"(

		set a to "Hello world!"
		set b to 5.5
		set c to 123
		set d to true
		set e to null

	)",
	
	u8R"(

		set a to 123
		set b to 234
		set c to a type
		set d to false
		if a type = b type
			set d to true
		end
		set e to false
		if a type = integer
			set e to true
		end

	)",
	
	u8R"(

		set a to -375.5
		set b to .11111
		set c to -999
		set d to 00001

	)",

	u8R"(

		-- Scope test - a will not be visible after end
		begin
			set a to 42
		end

		-- Scope test - b will be visible inside scope block
		set b to 999
		begin
			set b to 55
		end

	)",

	u8R"(

		-- Basic assignments 
		set a to 2
		set b to 3

		-- Basic math operations
		set c to a + b
		set d to b - a
		set e to a * b
		set f to b / 1
		set g to 10 % b

		-- Multiple operations with and without parentheses
		set h to 1 + 2 * 3     -- h = 9
		set i to 1 + (2 * 3)   -- i = 7

		-- Floating point assignments
		set j to 123.456
		set k to 234.567

		-- Floating point operations
		set l to j * k
		set m to j / k
		set n to j + k
		set o to j - k

	)",

	u8R"(

		set a to true = true		-- true
		set b to true != true		-- false
		set c to not true = true	-- false
		set d to true and false  	-- false
		set e to true or false		-- true

	)",

	u8R"(

		set a to 1 < 2				
		set b to 2 < 1	
			
		set c to 1 > 2				
		set d to 2 > 1	
			
		set e to 1 <= 2
		set f to 1 <= 1
		set g to 2 <= 1

		set h to 1 >= 2
		set i to 1 >= 1
		set j to 2 >= 1

	)",

	u8R"(

		-- Increment and decrement test
		set a to 1
		increment a
		set b to 1
		decrement b
		set c to 1
		increment c by 4
		set d to 1
		decrement d by 3
		set e to 1
		increment e by 4 * (2 + 2)

	)",

	u8R"(

		set a to 123.456 as integer
		set b to true as string
		set c to "false" as boolean
		set d to "456" as integer
		set e to "-123.456" as number
		set f to (4 + 5 + 6) as string

	)",

	u8R"(

		set a to 12345
		set b to a as string
		set c to b as integer
		set d to true
		set e to d as string
		set f to e as boolean
		set g to false
		if g type = boolean and g type = d type
			set g to true
		end

	)",

	u8R"(

		-- Simple if/else tests

		set a to false
		if true
			set a to true
		end

		set b to false
		if true
			set b to true
		else
			set b to false
		end

		set c to false
		if false
			set c to false
		else
			set c to true
		end

		set d to false
		if false
			set d to false
		else if true
			set d to true
		else
			set d to false
		end

		set e to false
		if false
			set e to false
		else if false
			set e to false
		else if true
			set e to true
		else
			set e to false
		end

		set f to false
		if false
			set f to false
		else if false
			set f to false
		else if false
			set f to false
		else
			set f to true
		end

		set g to false
		if true
			if true
				set g to true
			end
		end

		set h to false
		if false
		else
			if true
				set h to true
			end
		end

	)",
	
	u8R"(

		set a to 1
		loop while a < 10
			increment a
		end	
		
		set b to 1
		loop while true
			increment b
			if b = 10
				break
			end
		end	

	)",
	
	u8R"(

		set a to 1
		loop
			increment a
		while a < 10
		
		set b to 1
		loop
			increment b
			if b = 10
				break
			end
		while true

	)",

	u8R"(

		set a to 0
		loop from 1 to 10
			increment a
		end	

		set b to 0
		loop from 1 to 10 by 2
			increment b
		end	

		set c to 0
		loop from 10 to 1
			increment c
		end	

		set d to 0
		loop i from 1 to 3
			increment d by i
		end

		set e to 0
		loop from 1 to 10
			increment e
			break
		end
		
		set f to 0
		loop from 1 to 10
			increment f
			if f = 5
				break
			end
		end

	)",

	u8R"(

		set a to 0
		loop over 1, 2, 3
			increment a
		end

		set b to 0
		set x to 1, 2, 3
		loop over x
			increment b
		end

		set c to 0
		loop i over 1, 2, 3
			increment c by i value
		end

	)",

	u8R"(

		-- Create empty collection
		set a to []

	)",

	u8R"(

		-- Create collection using an initialization list
		set a to 3, 2, 1

	)",

	u8R"(

		-- Create collection using an initialization list of key-value pairs		
		set a to [1, "red"], [2, "green"], [3, "blue"]

	)",

	u8R"(

		-- Create collection using an initialization list of key-value pairs		
		set a to [1, "red"], [2, "green"], [3, "blue"]
			
		-- Change one of the elements by index
		set a[2] to "magenta"

	)",

	u8R"(

		-- Create collection using an initialization list of key-value pairs		
		set a to [1, "red"], [2, "green"], [3, "blue"]
			
		-- Set variable to one of the collection values
		set b to a[2]

	)",

	u8R"(

		-- Create collection using an initialization list of key-value pairs		
		set a to [1, "red"], [2, "green"], [3, "blue"]
			
		-- Add single element to a
		set a [4] to "purple"

	)",

	u8R"(

		-- Create collection using an initialization list of key-value pairs		
		set a to [1, "red"], [2, "green"], [3, "blue"]
			
		-- Set variable to one of the collection values
		set b to [4, "purple"]

	)",

	u8R"(

		-- Create collection using an initialization list of key-value pairs		
		set a to [1, "red"], [2, "green"], [3, "blue"]
			
		-- Remove element by key
		erase a[2]

	)",

	u8R"(

		set resumé to "my resumé text" 	
		set いろは to "いろはにほへとちりぬるをわかよたれそつねならむうゐのおくやまけふこえてあさきゆめみしゑひもせす"
		set Üben to "Falsches Üben von Xylophonmusik quält jeden größeren Zwerg"
		set Да to "В чащах юга жил бы цитрус? Да, но фальшивый экземпляр!"

	)",

	u8R"(

		import core

		function {integer x} is divisible by {integer y}
			return x % y = 0
		end

		loop i from 1 to 100
			set print number to true
			if i is divisible by 3
				write "Fizz"
				set print number to false
			end
			if i is divisible by 5
				write "Buzz"
				set print number to false
			end
			if print number
				write i
			end
			write newline
			wait
		end

	)",

	u8R"(

		function some values
			return "wolf", "goat", "cabbage"
		end

		set wolf to some values [1]
		set goat to some values [2]
		set cabbage to some values [3]

	)",

	u8R"(

		function do nothing/nada/ziltch
			-- do nothing
		end

		do nothing
		do nada
		do ziltch

	)",

	u8R"(

		function (a) b/c/d (e)
			-- do nothing
		end

		a b e
		a c e
		a d e
		b e
		c e
		d e
		b
		c
		d			

	)",

	u8R"(

		import core

		function {w} foo {x}
			return x + w
		end

		function bar {x} buzz
			return x
		end

		set a to bar function {} foo {} buzz

	)",

	u8R"(

		import core

		-- Function definition
		function test
		end

		-- Execute function asynchronously and store coroutine in variable c
		set c to async call function test

		-- Wait until coroutine is finished
		wait until c is finished

		-- Retrieve return value from coroutine
		set v to c's value

	)",

	u8R"(

		import core

		-- Function definition
		function test {x}
			return x
		end

		-- Execute function asynchronously and store coroutine in variable c
		set c to async call function test {} with 123

		-- Wait until coroutine is finished
		wait until c is finished

		-- Retrieve return value from coroutine
		set v to c's value

	)",

	u8R"(

		set a to "いろは"
		set a[1] to "は"
		set a[3] to "い"

	)",

};
		

class Fuzzer
{
public:
	const char * Fuzz(const char * str, int seed)
	{
		m_rng.seed(static_cast<unsigned int>(seed));
		double mutation = std::uniform_real_distribution<double>(0.0, 1.0)(m_rng);
		mutation = mutation * mutation * mutation;
		m_string = str;
		for (size_t i = 0; i < m_string.length(); ++i)
		{
			double rnd = std::uniform_real_distribution<double>(0.0, 1.0)(m_rng);
			if (rnd < mutation)
			{
				uint32_t rval = std::uniform_int_distribution<uint32_t>(1, 255)(m_rng);
				m_string[i] = static_cast<char>(rval);
			}
		}
		return m_string.c_str();
	}

private:
	std::mt19937 m_rng;
	std::string m_string;
};


int main(int argc, char * argv[])
{
	// Handle command-line options
	uint64_t permutations = 0;
	uint64_t start = 0;
	uint32_t minutes = 0;
	uint32_t hours = 0;
	bool showHelp = false;
	auto parser =
		Opt(permutations, "permutations")["-p"]["--permutations"]("Max number of permutations to run") |
		Opt(start, "start")["-s"]["--start"]("Starting permutation") |
		Opt(minutes, "minutes")["-m"]["--minutes"]("Minutes to run fuzz tests") |
		Opt(hours, "hours")["-h"]["--hours"]("Hours to run fuzz tests") |
		Help(showHelp);

	auto result = parser.parse(Args(argc, argv));
	if (!result)
	{
		std::cerr << "Error in command line: " << result.errorMessage() << "\n";
		return 1;
	}
	else if (showHelp)
	{
		parser.writeToStream(std::cout);
		return 0;
	}

	// Set max permutations and/or time to run
	if (!permutations && !minutes && !hours)
	{
		// Pick a default number of permutatiosn to run if no arguments are passed
		permutations = 100000;
		minutes = std::numeric_limits<uint32_t>::max();
	}
	else
	{
		if (hours)
			minutes += (hours * 60);
		if (minutes)
		{
			if (!permutations)
				permutations = std::numeric_limits<uint64_t>::max();
		}
	}

	// Start timer
	const auto startTime = std::chrono::high_resolution_clock::now();
	const auto stopTime = startTime + std::chrono::minutes(minutes);

	Jinx::GlobalParams globalParams;
	globalParams.logSymbols = false;
	globalParams.logBytecode = false;
	globalParams.enableLogging = false;
	Jinx::Initialize(globalParams);
	auto runtime = CreateRuntime();

	// Validate that all scripts compile and execute successfully
	for (auto i = 0u; i < std::size(s_testScripts); ++i)
	{
		// Compile the text to bytecode
		auto bytecode = runtime->Compile(s_testScripts[i], "Test Script", { "core" });
		assert(bytecode);

		// Create a runtime script with the given bytecode
		auto script = runtime->CreateScript(bytecode);
		assert(bytecode);
		assert(script->Execute());
	}

	std::list<std::thread> threadList;
	int numCores = std::thread::hardware_concurrency();
	for (int n = 0; n < numCores; ++n)
	{
		threadList.push_back(std::thread([n, numCores, permutations, runtime, stopTime]()
		{
			Fuzzer sourceFuzzer;
			for (int j = n; j < permutations; j += numCores)
			{
				if (std::chrono::high_resolution_clock::now() > stopTime)
					break;
				for (int i = 0; i < static_cast<int>(std::size(s_testScripts)); ++i)
				{
					// Compile the text to bytecode
					auto bytecode = runtime->Compile(sourceFuzzer.Fuzz(s_testScripts[i], j), "Test Script");
					if (!bytecode)
						continue;

					// Create and execute the script if viable
					auto script = runtime->CreateScript(bytecode);
					if (!script)
						continue;
					do
					{
						if (!script->Execute())
							break;
					}
					while (!script->IsFinished());
				}
				auto stats = Jinx::GetMemoryStats();
				printf("Permutation %i (Allocated Memory = %" PRIu64 ")\n", j, stats.allocatedMemory);
			}
		}));
	}

	// Wait for all threads to finish
	for (auto & t : threadList)
		t.join();
	threadList.clear();

	return 0;
}
