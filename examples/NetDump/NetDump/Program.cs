using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Divert.Net;

namespace NetDump
{
    class Program
    {
        static void Main(string[] args)
        {
            
            Diversion d = Diversion.Open("some filter", DivertLayer.Network, 100, FilterFlags.NoChecksum | FilterFlags.Drop);

            byte[] someBuffer = new byte[10];
            Address add = new Address();
            uint someNumber = 0;
            d.Receive(someBuffer, add, ref someNumber);

            
            d.ReceiveAsync(someBuffer, add, ref someNumber);
        }
    }
}
