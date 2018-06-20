using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tool.TransferPanel
{
    public struct TaskDetailinfo
    {
        private Int32 id;
        private UInt64 taskid;
        private string name;
        private string targetEp;
        private Int32 taskStatus;
        private UInt32 taskStep;

        public UInt64 TaskId
        {
            get { return taskid; }
            set
            {
                taskid = value;
            }
        }

        public Int32 Id
        {
            get { return id; }
            set
            {
                id = value;
            }
        }

        public string Name
        {
            get { return name; }
            set
            {
                name = value;

            }
        }

        public string TargetEp
        {
            get { return targetEp; }
            set
            {
                targetEp = value;

            }
        }

        public Int32 TaskStatus
        {
            get { return taskStatus; }
            set
            {
                taskStatus = value;
            }
        }

        public UInt32 TaskStep
        {
            get { return taskStep; }
            set
            {
                taskStep = value;
            }
        }
    }
}
