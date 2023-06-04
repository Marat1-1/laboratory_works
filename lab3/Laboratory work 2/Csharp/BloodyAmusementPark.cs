using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading;

namespace Csharp
{
    public partial class BloodyAmusementPark : Form
    {
        [DllImport("TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern void SendCommand(string nameClient, Command command);
        public BloodyAmusementPark()
        {
            InitializeComponent();
        }

        private EventWaitHandle eventCommand = new EventWaitHandle(false, EventResetMode.AutoReset, "CommandEvent");
        private EventWaitHandle eventConfirm = new EventWaitHandle(false, EventResetMode.AutoReset, "ConfirmEvent");

        private bool conditionServer = false;

        private Process server = null;

        private Dictionary<string, Form> Clients = new Dictionary<string, Form>();

        private void buttonOnServer_Click(object sender, EventArgs e)
        {
            if (server == null || server.HasExited)
            {
                server = Process.Start("Server.exe");
                conditionServer = true;
            }
        }

        private void buttonOffServer_Click(object sender, EventArgs e)
        {
            if (conditionServer)
            {
                foreach (var Client in Clients)
                {
                    Client.Value.Close();
                }
                
                Clients.Clear();

                SendCommand(this.Text, Command.CloseDialogue);
                eventCommand.Set();
                eventConfirm.WaitOne();
                conditionServer = false;
            }
        }

        private void buttonCreateClient_Click(object sender, EventArgs e)
        {
            if (conditionServer)
            {
                Clients.Add(textBox1.Text, new Client(textBox1.Text));
                Clients[textBox1.Text].Show();
            }
        }

        private void buttonDeleteClient_Click(object sender, EventArgs e)
        {
            if (conditionServer)
            {
                Clients[textBox1.Text].Close();
                Clients.Remove(textBox1.Text);
            }
        }
    }
}
