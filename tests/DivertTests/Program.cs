/*
* This software is ported from the original test application for WinDivert,
* which can be found at https://github.com/basil00/Divert/blob/master/test/.
* The License for this software remains under the license of the original
* Author. Note however that the tests are under a different license than the
* CLR library.
*
* Program.cs, ported from test_data.c and test.c
* Copyright © 2015 basil (reqrypt.org), all rights reserved
* Copyright © 2015 Jesse Nicholson
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

using DivertTests.Tests;
using Newtonsoft.Json;
using Divert.Net;
using System.IO;

namespace DivertTests
{
    internal class Program
    {
        private static readonly uint MaxPacket = 2048;       

        private static void Main(string[] args)
        {
            string testsFilePath = System.AppDomain.CurrentDomain.BaseDirectory + @"TestData\Tests.json";
            if (!File.Exists(testsFilePath))
            {
                System.Console.WriteLine(string.Format("Tests JSON file does not exist where expected at {0}\n", testsFilePath));
                return;
            }

            string testData = File.ReadAllText(testsFilePath);
            
            System.Collections.Generic.List<Test> tests = new System.Collections.Generic.List<Test>();

            Newtonsoft.Json.Linq.JToken token = Newtonsoft.Json.Linq.JObject.Parse(testData);
            
            foreach(var tok in token.SelectToken("Tests"))
            {
                string name = (string)tok.SelectToken("TestName");
                string filter = (string)tok.SelectToken("TestFilter");
                string data = (string)tok.SelectToken("TestData");
                string match = (string)tok.SelectToken("Match");

                tests.Add(new Test(name, filter, data, match));
            }

            // Open handles to:
            // (1) stop normal traffic from interacting with the tests; and
            // (2) stop test packets escaping to the Internet or TCP/IP stack.
            Diversion upper;
            Diversion lower; 

            try
            {
                upper = Diversion.Open("true", DivertLayer.Network, -510, FilterFlags.Drop);
            }catch(System.Exception e)
            {
                System.Console.WriteLine(string.Format("Failed to open upper Diversion with error {0}\n", e.Message));
                return;
            }

            try
            {
                lower = Diversion.Open("true", DivertLayer.Network, 510, FilterFlags.Drop);
            }
            catch (System.Exception e)
            {
                System.Console.WriteLine(string.Format("Failed to open lower Diversion with error {0}\n", e.Message));
                return;
            }

            if(!upper.Handle.Valid)
            {
                System.Console.WriteLine(string.Format("Failed to open upper Diversion with error {0}\n", System.Runtime.InteropServices.Marshal.GetLastWin32Error()));
                return;
            }

            if (!lower.Handle.Valid)
            {
                System.Console.WriteLine(string.Format("Failed to open lower Diversion with error {0}\n", System.Runtime.InteropServices.Marshal.GetLastWin32Error()));
                return;
            }

            // Wait for existing packets to flush:
            System.Threading.Thread.Sleep(100);

            int testsPassed = 0;
            int testsFailed = 0;

            // Run tests:
            for(int i = 0; i < tests.Count; ++i)
            {
                var test = tests[i];

                // Ensure the correct checksum
                upper.CalculateChecksums(test.TestPacketData, (uint)test.TestPacketData.Length, 0);

                bool testResult = RunTest(test, upper);

                if(testResult)
                {
                    System.Console.BackgroundColor = System.ConsoleColor.Green;
                    System.Console.ForegroundColor = System.ConsoleColor.White;
                    System.Console.WriteLine(test.TestName + " Passed.");
                    System.Console.ResetColor();
                    testsPassed++;
                }
                else
                {
                    System.Console.BackgroundColor = System.ConsoleColor.Red;
                    System.Console.ForegroundColor = System.ConsoleColor.White;
                    System.Console.WriteLine(test.TestName + " Failed!");
                    System.Console.ResetColor();
                    testsFailed++;
                }
            }

            if (!upper.Close())
            {
                System.Console.WriteLine("Failed to close upper handle");
            }

            if (!lower.Close())
            {
                System.Console.WriteLine("Failed to close lower handle");
            }

            System.Console.WriteLine("{0} tests passed and {1} tests failed.", testsPassed, testsFailed);
        }

        private static bool RunTest(Test test, Diversion injectionDiversion)
        {
            byte[] buffer = new byte[MaxPacket];

            string validationError = string.Empty;

            if(!Diversion.ValidateFilter(test.Filter, DivertLayer.Network, ref validationError))
            {
                System.Console.WriteLine("Error in filter string {0}", validationError);
                return false;
            }

            Address address = new Address();
            address.Direction = DivertDirection.Outbound;

            if(Diversion.EvaluateFilter(test.Filter, DivertLayer.Network, test.TestPacketData, (uint)test.TestPacketData.Length, address) != test.Match)
            {
                System.Console.WriteLine("Error - Filter {0} does not match the packet.", test.Filter);
                return false;
            }

            Diversion testDiversion1 = null;

            Diversion testDiversion2 = null;

            try
            {
                // (1) Open a WinDivert handle to the given filter
                testDiversion1 = Diversion.Open(test.Filter, DivertLayer.Network, 0, 0);
            }
            catch (System.Exception e)
            {
                System.Console.WriteLine(string.Format("Failed to open testDiversion1 with error {0}\n", e.Message));
                return false;
            }

            if (!testDiversion1.Handle.Valid)
            {
                System.Console.WriteLine(string.Format("Failed to open testDiversion1 with error {0}.\n", System.Runtime.InteropServices.Marshal.GetLastWin32Error()));
                return false;
            }

            if(!test.Match)
            {
                testDiversion2 = testDiversion1;

                try
                {
                    // Catch non-matching packets
                    testDiversion1 = Diversion.Open("true", DivertLayer.Network, 33, 0);
                }
                catch (System.Exception e)
                {
                    System.Console.WriteLine(string.Format("Failed to open testDiversion2 with error {0}\n", e.Message));
                    testDiversion1.Close();
                    return false;
                }

                if (!testDiversion2.Handle.Valid)
                {
                    System.Console.WriteLine(string.Format("Failed to open testDiversion2 with error {0}.\n", System.Runtime.InteropServices.Marshal.GetLastWin32Error()));
                    testDiversion1.Close();
                    return false;
                }
            }

            // (2) Inject the packet
            uint injectLength = 0;
            if(!injectionDiversion.Send(test.TestPacketData, (uint)test.TestPacketData.Length, address, ref injectLength))
            {
                System.Console.WriteLine(string.Format("Error injecting test packet with error {0}.\n", System.Runtime.InteropServices.Marshal.GetLastWin32Error()));

                if (testDiversion2 != null)
                {
                    testDiversion2.Close();
                }
                testDiversion1.Close();

                return false;
            }

            // (3) Wait for the packet to arrive
            DivertAsyncResult result = new DivertAsyncResult();
            uint recvLength = 0;
            if(!testDiversion1.ReceiveAsync(buffer, address, ref recvLength, result))
            {
                if(!result.NoError)
                {
                    System.Console.WriteLine(string.Format("Async packet reception failed with error {0}.\n", result.ErrorCode));

                    if (testDiversion2 != null)
                    {
                        testDiversion2.Close();
                    }
                    testDiversion1.Close();

                    return false;
                }

                if(!result.Get(250))
                {
                    System.Console.WriteLine(string.Format("Async packet read failed with error {0}.\n", result.ErrorCode));

                    if (testDiversion2 != null)
                    {
                        testDiversion2.Close();
                    }
                    testDiversion1.Close();

                    return false;
                }

                recvLength = result.Length;
            }

            if(address.Direction == DivertDirection.Outbound)
            {
                testDiversion1.CalculateChecksums(buffer, recvLength, 0);
            }

            // (4) Verify that the packet is the same
            if(recvLength != test.TestPacketData.Length)
            {
                System.Console.WriteLine("Received packet does not match test packet.\n");

                if (testDiversion2 != null)
                {
                    testDiversion2.Close();
                }
                testDiversion1.Close();

                return false;
            }

            for(int i = 0; i < test.TestPacketData.Length; ++i)
            {
                if(buffer[i] != test.TestPacketData[i])
                {
                    System.Console.WriteLine("Received packet does not match test packet.\n");

                    if (testDiversion2 != null)
                    {
                        testDiversion2.Close();
                    }
                    testDiversion1.Close();

                    return false;
                }
            }

            // (5) Clean-up
            if (testDiversion1 != null)
            {
                testDiversion1.Close();
            }
                        
            if (testDiversion2 != null)
            {
                testDiversion2.Close();
            }

            return true;
        }
    }
}