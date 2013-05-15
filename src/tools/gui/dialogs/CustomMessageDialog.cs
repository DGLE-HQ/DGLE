/**
\author		Shestakov Mikhail aka MIKE
\date		29.10.2012 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;

namespace Gui
{
	public partial class CustomMessageDialog : Gui.CustomWindow
	{
		private static int ICON_SIZE = 48;

		public string Text
		{
			get { return lText.Text; }
			set { lText.Text = value; }
		}

		public Gtk.MessageType MessageType
		{
			get { return messageType; }
			set
			{
				messageType = value;
				SetIcon();
			}
		}
		private Gtk.MessageType messageType;

		public Gtk.ButtonsType ButtonsType
		{
			get { return buttonsType; }
			set
			{
				buttonsType = value;
				SetButtons();
			}
		}
		private Gtk.ButtonsType buttonsType;

		public event EventHandler Ok;
		public event EventHandler Cancel;
		public event EventHandler Close;
		public event EventHandler Yes;
		public event EventHandler No;

		public CustomMessageDialog(
			Gtk.Window parent, 
			Gtk.MessageType messageType,
			string format,
			params object[] args) :
			this(parent, messageType, Gtk.ButtonsType.Ok, "", String.Format(format, args))
		{
		}

		public CustomMessageDialog(
			Gtk.Window parent, 
			Gtk.MessageType messageType, 
			Gtk.ButtonsType buttonsType, 
			string format,
			params object[] args) :
			this(parent, messageType, buttonsType, "", String.Format(format, args))
		{
		}

		public CustomMessageDialog(
			Gtk.Window parent, 
			Gtk.MessageType messageType, 
			Gtk.ButtonsType buttonsType, 
			string title,
			string format,
			params object[] args) :
			this(parent, messageType, buttonsType, title, String.Format(format, args))
		{
		}

		public CustomMessageDialog(
			Gtk.Window parent, 
			Gtk.MessageType messageType, 
			Gtk.ButtonsType buttonsType, 
			string title, string text) : base(Gtk.WindowType.Toplevel)
		{
			this.Build();

			if (parent != null)
			{
				base.TransientFor = parent;
				base.SetPosition(Gtk.WindowPosition.CenterOnParent);
				base.Decorated = parent.Decorated;
			}
			else
			{
				base.SetPosition(Gtk.WindowPosition.Center);
			}
			base.KeepAbove = true;
			
			base.Title = title;
			Text = text;
			MessageType = messageType;
			ButtonsType = buttonsType;

			this.btnPositive.Clicked += HandlePositiveClicked;
			this.btnNegative.Clicked += HandleNegativeClicked;
			this.DeleteEvent += delegate(object o, Gtk.DeleteEventArgs args)
			{
				if (btnNegative.Visible)
					this.btnNegative.Click();
				else
					this.btnPositive.Click();
			};
		}

		void HandlePositiveClicked(object sender, EventArgs e)
		{
			if (Ok != null)
				Ok(sender, e);
			if (Yes != null)
				Yes(sender, e);

			this.Hide();
		}

		void HandleNegativeClicked(object sender, EventArgs e)
		{
			if (Cancel != null)
				Cancel(sender, e);
			if (Close != null)
				Close(sender, e);
			if (No != null)
				No(sender, e);

			this.Hide();
		}

		private void SetIcon()
		{
			if ((int)messageType < 0 || (int)messageType >= (int)Gtk.MessageType.Other)
				this.imageType.Pixbuf = null;
			else
			{
				string iconName = String.Format("gtk-dialog-{0}", messageType.ToString()).ToLower();
				if (null != Gtk.IconTheme.Default.LookupIcon(iconName, ICON_SIZE, Gtk.IconLookupFlags.UseBuiltin))
					this.imageType.Pixbuf =
						Gtk.IconTheme.Default.LoadIcon(iconName, ICON_SIZE, Gtk.IconLookupFlags.UseBuiltin);
				else
					this.imageType.Pixbuf = null;
			}
		}

		private void SetButtons()
		{
			switch (buttonsType)
			{
				case Gtk.ButtonsType.Ok:
					btnPositive.Label = "Ok";
					btnNegative.Visible = false;
					btnNegative.Sensitive = false;
					break;
				case Gtk.ButtonsType.Close:
					btnPositive.Visible = false;
					btnPositive.Sensitive = false;
					btnNegative.Label = "Close";
					break;
				case Gtk.ButtonsType.Cancel:
					btnPositive.Visible = false;
					btnPositive.Sensitive = false;
					btnNegative.Label = "Cancel";
					break;
				case Gtk.ButtonsType.OkCancel:
					btnPositive.Label = "Ok";
					btnNegative.Label = "Cancel";
					break;
				case Gtk.ButtonsType.YesNo:
					btnPositive.Label = "Yes";
					btnNegative.Label = "No";
					break;
				default:
					this.hbtnboxAction.Visible = false;
					this.hbtnboxAction.Sensitive = false;
					break;
			}
		}

		public void Run()
		{
			Gtk.Application.Invoke((sender, e) => this.Show());
			do
			{
				while (Gtk.Application.EventsPending())
					Gtk.Application.RunIteration();

				System.Threading.Thread.Sleep(50); 
			}
			while(this.Visible);
		}
	}
}

