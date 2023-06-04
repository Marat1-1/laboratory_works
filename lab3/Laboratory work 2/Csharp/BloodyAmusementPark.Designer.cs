namespace Csharp
{
    partial class BloodyAmusementPark
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.buttonOnServer = new System.Windows.Forms.Button();
            this.buttonOffServer = new System.Windows.Forms.Button();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.buttonCreateClient = new System.Windows.Forms.Button();
            this.buttonDeleteClient = new System.Windows.Forms.Button();
            this.label5 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // buttonOnServer
            // 
            this.buttonOnServer.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.buttonOnServer.Font = new System.Drawing.Font("Consolas", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.buttonOnServer.Location = new System.Drawing.Point(30, 35);
            this.buttonOnServer.Name = "buttonOnServer";
            this.buttonOnServer.Size = new System.Drawing.Size(109, 63);
            this.buttonOnServer.TabIndex = 0;
            this.buttonOnServer.Text = "Включить сервер";
            this.buttonOnServer.UseVisualStyleBackColor = false;
            this.buttonOnServer.Click += new System.EventHandler(this.buttonOnServer_Click);
            // 
            // buttonOffServer
            // 
            this.buttonOffServer.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.buttonOffServer.Font = new System.Drawing.Font("Consolas", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.buttonOffServer.Location = new System.Drawing.Point(30, 104);
            this.buttonOffServer.Name = "buttonOffServer";
            this.buttonOffServer.Size = new System.Drawing.Size(109, 63);
            this.buttonOffServer.TabIndex = 1;
            this.buttonOffServer.Text = "Отключить сервер";
            this.buttonOffServer.UseVisualStyleBackColor = false;
            this.buttonOffServer.Click += new System.EventHandler(this.buttonOffServer_Click);
            // 
            // textBox1
            // 
            this.textBox1.Font = new System.Drawing.Font("Consolas", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.textBox1.Location = new System.Drawing.Point(30, 210);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(254, 31);
            this.textBox1.TabIndex = 4;
            this.textBox1.Text = "1000-7";
            this.textBox1.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // buttonCreateClient
            // 
            this.buttonCreateClient.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.buttonCreateClient.Font = new System.Drawing.Font("Consolas", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.buttonCreateClient.Location = new System.Drawing.Point(175, 35);
            this.buttonCreateClient.Name = "buttonCreateClient";
            this.buttonCreateClient.Size = new System.Drawing.Size(109, 63);
            this.buttonCreateClient.TabIndex = 7;
            this.buttonCreateClient.Text = "Сотворить клиента";
            this.buttonCreateClient.UseVisualStyleBackColor = false;
            this.buttonCreateClient.Click += new System.EventHandler(this.buttonCreateClient_Click);
            // 
            // buttonDeleteClient
            // 
            this.buttonDeleteClient.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.buttonDeleteClient.Font = new System.Drawing.Font("Consolas", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.buttonDeleteClient.Location = new System.Drawing.Point(175, 104);
            this.buttonDeleteClient.Name = "buttonDeleteClient";
            this.buttonDeleteClient.Size = new System.Drawing.Size(109, 63);
            this.buttonDeleteClient.TabIndex = 8;
            this.buttonDeleteClient.Text = "Стереть клиента";
            this.buttonDeleteClient.UseVisualStyleBackColor = false;
            this.buttonDeleteClient.Click += new System.EventHandler(this.buttonDeleteClient_Click);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Consolas", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label5.Location = new System.Drawing.Point(92, 185);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(120, 22);
            this.label5.TabIndex = 9;
            this.label5.Text = "Имя клиента";
            // 
            // BloodyAmusementPark
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Window;
            this.ClientSize = new System.Drawing.Size(308, 253);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.buttonDeleteClient);
            this.Controls.Add(this.buttonCreateClient);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.buttonOffServer);
            this.Controls.Add(this.buttonOnServer);
            this.Name = "BloodyAmusementPark";
            this.Text = "Парк кровавых атракционов";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonOnServer;
        private System.Windows.Forms.Button buttonOffServer;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Button buttonCreateClient;
        private System.Windows.Forms.Button buttonDeleteClient;
        private System.Windows.Forms.Label label5;
    }
}