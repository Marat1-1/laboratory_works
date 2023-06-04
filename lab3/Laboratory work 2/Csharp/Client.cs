using System;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using System.Threading;
using System.Runtime.InteropServices;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

enum Command {
    Connection = 0,
    DisconnectingConnection = 1,
    Start = 2,
    Stop = 3,
    Send = 4,
    CloseDialogue = 5,
    SendMessageServer = 6
}

namespace Csharp
{
    public partial class Client : Form
    {
        [DllImport("TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern void SendCommand(string nameClient, Command command);

        [DllImport("TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern void SendConnectionRequest(string nameClient);

        [DllImport("TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern void SendDisconnectRequest(string nameClient);

        [DllImport("TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern void SendNumberThread(string nameClient, int numberThread);

        [DllImport("TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern void SendSMS(string nameClient, string message);

        //private Process CplusplusProcess = null;
        //private EventWaitHandle stopEvent = new EventWaitHandle(false, EventResetMode.AutoReset, "StopEvent");
        //private EventWaitHandle startEvent = new EventWaitHandle(false, EventResetMode.AutoReset, "StartEvent");
        //private EventWaitHandle sendEvent = new EventWaitHandle(false, EventResetMode.AutoReset, "SendEvent");
        private EventWaitHandle eventCommand = new EventWaitHandle(false, EventResetMode.AutoReset, "CommandEvent");
        private EventWaitHandle eventConfirm = new EventWaitHandle(false, EventResetMode.AutoReset, "ConfirmEvent");

        public Client(string nameClient)
        {
            InitializeComponent();
            this.Text = nameClient;
        }

        private int countCreateThreads = 0;

        private bool connection = false;

        private void Disconnected()
        {
            StopAllThreads();
            //for (; !(countThreads < 0); --countThreads)
            //{
            //    if (countThreads != 0)
            //    {
            //        //SendCommand(Command.Stop);
            //        eventConfirm.WaitOne();
            //        comboBox1.Items.Remove("Thread " + countThreads.ToString());
            //    }
            //    else
            //    {
            //        comboBox1.Items.Remove("Main thread");
            //        //SendCommand(Command.DisconnectingConnection);
            //        eventConfirm.WaitOne();
            //        break;
            //    }
            //}
            SendCommand(this.Text, Command.DisconnectingConnection);
            eventCommand.Set();
            eventConfirm.WaitOne();
            SendDisconnectRequest(this.Text);
            eventCommand.Set();
            eventConfirm.WaitOne();
            comboBox1.Items.Remove("Все потоки");
            comboBox1.Items.Remove("Сервер");
            countCreateThreads = 0;
            connection = false;
        }

        private void StopThread(int numberThread)
        {
            SendCommand(this.Text, Command.Stop);
            eventCommand.Set();
            eventConfirm.WaitOne();
            SendNumberThread(this.Text, numberThread);
            eventCommand.Set();
            eventConfirm.WaitOne();
        }

        private void StopAllThreads()
        {
            while (comboBox1.Items.Count != 2)
            {
                StopThread(int.Parse(comboBox1.Items[comboBox1.Items.Count - 1].ToString().Substring(6)));
                comboBox1.Items.RemoveAt(comboBox1.Items.Count - 1);
            }
        }

        private void SendMessageServer()
        {
            SendCommand(this.Text, Command.SendMessageServer);
            eventCommand.Set();
            eventConfirm.WaitOne();

            SendSMS(this.Text, textBox1.Text);
            eventCommand.Set();
            eventConfirm.WaitOne();

        }

        private void SendSMS(int numberThread)
        {
            SendCommand(this.Text, Command.Send);
            eventCommand.Set();
            eventConfirm.WaitOne();

            SendNumberThread(this.Text, numberThread);
            eventCommand.Set();
            eventConfirm.WaitOne();

            SendSMS(this.Text, textBox1.Text);
            eventCommand.Set();
            eventConfirm.WaitOne();
        }

        private bool getNumberThread(ref int numberThread)
        {
            if ((comboBox1.Text != string.Empty) && (int.TryParse(comboBox1.Text.Substring(6), out numberThread)))
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        private void buttonStart_Click(object sender, EventArgs e)
        {
            if (connection)
            {            
                for (int i = 0; i < numericUpDown1.Value; ++i)
                {
                    //bool asdf = eventConfirm.WaitOne(0);
                    SendCommand(this.Text, Command.Start);
                    eventCommand.Set();
                    eventConfirm.WaitOne();
                    ++countCreateThreads;
                    comboBox1.Items.Add("Поток " + countCreateThreads.ToString());
                }               
            }
        }

        private void buttonStop_Click(object sender, EventArgs e)
        {
            if (connection)
            {
                if (comboBox1.Text == "Все потоки")
                {
                    StopAllThreads();
                }
                if (countCreateThreads != 0)
                {
                    int numberThread = -1;
                    if (getNumberThread(ref numberThread))
                    {
                        StopThread(numberThread);
                        //--countThreads;
                        comboBox1.Items.Remove("Поток " + numberThread);
                    }
                }
                //else
                //{
                //    //SendCommand(Command.CloseMainThread);
                //    eventConfirm.WaitOne();
                //    break;
                //}
            }            
        }

        private void buttonSend_Click(object sender, EventArgs e)
        {
            if (connection)
            {
                int numberThread = -1;
                switch (comboBox1.Text)
                {
                    case "Сервер":

                        SendMessageServer();

                        break;

                    case "Все потоки":
                        {
                            int countSendSMS = 0; 
                            SendMessageServer();
                            while (comboBox1.Items.Count - countSendSMS != 2)
                            {
                                SendSMS(int.Parse(comboBox1.Items[countSendSMS + 2].ToString().Substring(6)));
                                ++countSendSMS;
                            }
                            break;
                        }
                    default:
                        if (getNumberThread(ref numberThread))
                        {
                            SendSMS(numberThread);
                        }
                        break;
                }
            }
        }

        private void Connect_Click(object sender, EventArgs e)
        {
            if (!connection)
            {
                //if (CplusplusProcess == null || CplusplusProcess.HasExited)
                //{
                //    countCreateThreads = 0;
                //    comboBox1.Items.Clear();

                //    CplusplusProcess = Process.Start("Cplusplus.exe");
                //}
                countCreateThreads = 0;
                comboBox1.Items.Clear();

                SendCommand(this.Text, Command.Connection);
                eventCommand.Set();
                eventConfirm.WaitOne();

                SendConnectionRequest(this.Text);
                eventCommand.Set();
                eventConfirm.WaitOne();
                
                comboBox1.Items.Add("Сервер");
                comboBox1.Items.Add("Все потоки");
                connection = true;
            }
        }

        private void Disconnect_Click(object sender, EventArgs e)
        {
            if (connection)
            {
                Disconnected();
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (connection)
            {
                Disconnected();
            }
            //if (!(CplusplusProcess == null || CplusplusProcess.HasExited))
            //{
            //    SendCommand(this.Text, Command.CloseDialogue);
            //    eventCommand.Set();
            //    eventConfirm.WaitOne();
            //}
        }


    } 
       
}





