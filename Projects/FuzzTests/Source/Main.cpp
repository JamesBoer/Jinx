/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include <assert.h>
#include <random>

#include "../../../Source/Jinx.h"

using namespace Jinx;

const size_t NumPermutations = 100000;

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

		a --- some comment --- is 123
   
	)",

	u8R"(

		a is "Hello world!"
		b is 5.5
		c is 123
		d is true
		e is null

	)",
	
	u8R"(

		a is 123
		b is 234
		c is a type
		d is false
		if a type = b type
			d is true
		end
		e is false
		if a type = integer
			e is true
		end

	)",
	
	u8R"(

		a is -375.5
		b is .11111
		c is -999
		d is 00001

	)",

	u8R"(

		-- Scope test - a will not be visible after end
		begin
			a is 42
		end

		-- Scope test - g will be visible inside scope block
		b is 999
		begin
			b is 55
		end

	)",

	u8R"(

		-- Basic assignments 
		a is 2
		b is 3

		-- Basic math operations
		c is a + b
		d is b - a
		e is a * b
		f is b / 1
		g is 10 mod b

		-- Multiple operations with and without parentheses
		h is 1 + 2 * 3     -- h is 9
		i is 1 + (2 * 3)   -- i is 7

		-- Floating point assignments
		j is 123.456
		k is 234.567

		-- Floating point operations
		l is j * k
		m is j / k
		n is j + k
		o is j - k

	)",

	u8R"(

		a is true = true		-- true
		b is true != true		-- false
		c is not true = true	-- false
		d is true and false  	-- false
		e is true or false		-- true

	)",

	u8R"(

		a is 1 < 2				
		b is 2 < 1	
			
		c is 1 > 2				
		d is 2 > 1	
			
		e is 1 <= 2
		f is 1 <= 1
		g is 2 <= 1

		h is 1 >= 2
		i is 1 >= 1
		j is 2 >= 1

	)",

	u8R"(

		-- Increment and decrement test
		a is 1
		increment a
		b is 1
		decrement b
		c is 1
		increment c by 4
		d is 1
		decrement d by 3
		e is 1
		increment e by 4 * (2 + 2)

	)",

	u8R"(

		a is 123.456 as integer
		b is true as string
		c is "false" as boolean
		d is "456" as integer
		e is "-123.456" as number
		f is 4 + 5 + 6 as string

	)",

	u8R"(

		a is 12345
		b is a as string
		c is b as integer
		d is true
		e is d as string
		f is e as boolean
		g is false
		if g type = boolean and g type = d type
			g is true
        end

	)",

	u8R"(

		-- Simple if/else tests

		a is false
		if true
			a is true
		end

		b is false
		if true
			b is true
		else
			b is false
		end

		c is false
		if false
			c is false
		else
			c is true
		end

		d is false
		if false
			d is false
		else if true
			d is true
		else
			d is false
		end

		e is false
		if false
			e is false
		else if false
			e is false
		else if true
			e is true
		else
			e is false
		end

		f is false
		if false
			f is false
		else if false
			f is false
		else if false
			f is false
		else
			f is true
		end

		g is false
		if true
			if true
				g is true
			end
		end

		h is false
		if false
		else
			if true
				h is true
			end
		end

	)",
	
	u8R"(

		a is 1
		loop while a < 10
			increment a
		end	
		
		b is 1
		loop while true
			increment b
			if b = 10
				break
			end
		end	

	)",
	
	u8R"(

		a is 1
		loop
			increment a
		while a < 10
		
		b is 1
		loop
			increment b
			if b = 10
				break
			end
		while true

	)",

	u8R"(

		a is 0
		loop from 1 to 10
			increment a
		end	

		b is 0
		loop from 1 to 10 by 2
			increment b
		end	

		c is 0
		loop from 10 to 1
			increment c
		end	

		d is 0
		loop i from 1 to 3
			increment d by i
		end

		e is 0
		loop from 1 to 10
			increment e
			break
		end
		
		f is 0
		loop from 1 to 10
			increment f
			if f = 5
				break
			end
		end

	)",

	u8R"(

		a is 0
		loop over 1, 2, 3
			increment a
		end

		b is 0
		x is 1, 2, 3
		loop over x
			increment b
		end

		c is 0
		loop i over 1, 2, 3
			increment c by i
		end

	)",

	u8R"(

		-- Create empty collection
		a is []

	)",

	u8R"(

		-- Create collection using an initialization list
		a is 3, 2, 1

	)",

	u8R"(

		-- Create collection using an initialization list of key-value pairs		
		a is [1, "red"], [2, "green"], [3, "blue"]

	)",

	u8R"(

		-- Create collection using an initialization list of key-value pairs		
		a is [1, "red"], [2, "green"], [3, "blue"]
			
		-- Change one of the elements by index
		a[2] is "magenta"

	)",

	u8R"(

		-- Create collection using an initialization list of key-value pairs		
		a is [1, "red"], [2, "green"], [3, "blue"]
			
		-- Set variable to one of the collection values
		b is a[2]

	)",

	u8R"(

		import core

		-- Create collection using an initialization list of key-value pairs		
		a is [1, "red"], [2, "green"], [3, "blue"]
			
		-- Add single element to a
		add "purple" to a

	)",

	u8R"(

		import core

		-- Create collection using an initialization list of key-value pairs		
		a is [1, "red"], [2, "green"], [3, "blue"]
			
		-- Set variable to one of the collection values
		b is [4, "purple"]

		-- Add elements in b to a
		add b to a

	)",

	u8R"(

		import core

		-- Create collection using an initialization list of key-value pairs		
		a is [1, "red"], [2, "green"], [3, "blue"]
			
		-- Remove element by key
		remove 2 from a

	)",

	u8R"(

		import core

		-- Create collection using an initialization list of key-value pairs		
		a is [1, "red"], [2, "green"], [3, "blue"]
			
		-- Remove elements by key
		remove (1, 2) from a

	)",

	u8R"(

		import core

		-- Create collection using an initialization list of key-value pairs		
		a is [1, "red"], [2, "green"], [3, "blue"]
			
		-- Remove element by value
		remove value "red" from a

	)",

	u8R"(

		import core

		-- Create collection using an initialization list of key-value pairs		
		a is [1, "red"], [2, "green"], [3, "blue"]
			
		-- Remove multiple elements by value
		remove values ("red", "green") from a

	)",

	u8R"(

		resumé is "my resumé text" 	
		いろは is "いろはにほへとちりぬるをわかよたれそつねならむうゐのおくやまけふこえてあさきゆめみしゑひもせす"
		Üben is "Falsches Üben von Xylophonmusik quält jeden größeren Zwerg"
		Да is "В чащах юга жил бы цитрус? Да, но фальшивый экземпляр!"

	)",

};
		

template<typename T, size_t s>
constexpr size_t countof(T(&)[s])
{
	return s;
}

class Fuzzer
{
public:
	Fuzzer(unsigned int seed = 0)
	{
		m_seed = seed;
	}
	const char * Fuzz(const char * str)
	{
		m_rng.seed(m_seed);
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
		++m_seed;
		return m_string.c_str();
	}

	BufferPtr Fuzz(BufferPtr buffer)
	{
		m_rng.seed(m_seed);
		double mutation = std::uniform_real_distribution<double>(0.0, 1.0)(m_rng);
		mutation = mutation * mutation * mutation;
		for (size_t i = 0; i < m_string.length(); ++i)
		{
			double rnd = std::uniform_real_distribution<double>(0.0, 1.0)(m_rng);
			if (rnd < mutation)
			{
				uint32_t rval = std::uniform_int_distribution<uint32_t>(1, 255)(m_rng);
				buffer->Ptr()[i] = static_cast<char>(rval);
			}
		}
		++m_seed;
		return buffer;
	}

private:
	std::mt19937 m_rng;
	std::string m_string;
	unsigned int m_seed;
};


int main(int argc, char * argv[])
{
	Jinx::GlobalParams globalParams;
	globalParams.logSymbols = false;
	globalParams.logBytecode = false;
	globalParams.enableLogging = false;
	Jinx::Initialize(globalParams);

	// Validate that all scripts compile and execute successfully
	for (auto i = 0u; i < countof(s_testScripts); ++i)
	{
		auto runtime = CreateRuntime();
		// Compile the text to bytecode
		auto bytecode = runtime->Compile(s_testScripts[i], "Test Script");
		assert(bytecode);

		// Create a runtime script with the given bytecode
		auto script = runtime->CreateScript(bytecode);
		if (script)
			script->Execute();
	}

	// Run fuzz permutations on source
	Fuzzer sourceFuzzer;
	for (auto c = 0; c < NumPermutations; ++c)
	{
		auto runtime = CreateRuntime();
		for (auto i = 0u; i < countof(s_testScripts); ++i)
		{
			// Compile the text to bytecode
			auto bytecode = runtime->Compile(sourceFuzzer.Fuzz(s_testScripts[i]), "Test Script");
			if (!bytecode)
				continue;

			// Create a runtime script with the given bytecode if it exists
			auto script = runtime->CreateScript(bytecode);
			if (script)
				script->Execute();
		}
		auto stats = Jinx::GetMemoryStats();
		printf("Source permutation %i (Allocated Memory = %lli)\n", c, stats.currentAllocatedMemory);
	}

	// Run fuzz permutations on bytecode
	Fuzzer bytecodeFuzzer;
	for (auto c = 0; c < NumPermutations; ++c)
	{
		auto runtime = CreateRuntime();
		for (auto i = 0u; i < countof(s_testScripts); ++i)
		{
			// Compile the text to bytecode
			auto bytecode = runtime->Compile(s_testScripts[i], "Test Script");
			if (!bytecode)
				continue;

			// Create a runtime script with the given bytecode if it exists
			auto script = runtime->CreateScript(bytecodeFuzzer.Fuzz(bytecode));
			assert(script);
			script->Execute();
		}
		auto stats = Jinx::GetMemoryStats();
		printf("Bytecode permutation %i (Allocated Memory = %lli)\n", c, stats.currentAllocatedMemory);
	}

	return 0;
}