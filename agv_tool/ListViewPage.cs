using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

namespace Tool
{
    class ListViewPage<T>
    {
        public List<T> DataSource_;
        int PageSize_;
        int PageCount_;
        int CurrentIndex_;

        public ListViewPage()
        {
            CurrentIndex_ = 1;
        }

        public ListViewPage(List<T> DataSource, int PageSize): this()
        {
            this.DataSource_ = DataSource;
            this.PageSize_ = PageSize;
            this.PageCount_ = DataSource.Count / PageSize;
            this.PageCount_ += (DataSource.Count % PageSize) != 0 ? 1 : 0;
        }

        public List<T> GetPageData(JumpOperation mode)
        {
            switch (mode)
            {
                case JumpOperation.GoHome:
                    CurrentIndex_ = 1;
                    break;
                case JumpOperation.GoPrePrevious:
                    if (CurrentIndex_ > 1)
                    {
                        CurrentIndex_ -= 1;
                    }
                    break;
                case JumpOperation.GoNext:
                    if (CurrentIndex_ < PageCount_)
                    {
                        CurrentIndex_ += 1;
                    }
                    break;
                case JumpOperation.GoEnd:
                    CurrentIndex_ = PageCount_;
                    break;
            }

            List<T> listPageData = new List<T>();
            try
            {
                int pageCountTo = PageSize_;
                if (PageCount_ == CurrentIndex_ && DataSource_.Count % PageSize_ > 0)
                {
                    pageCountTo = DataSource_.Count % PageSize_;
                }
                if (null != DataSource_)
                {
                    for (int i = 0; i < pageCountTo; i++)
                    {
                        if ((CurrentIndex_ - 1) * PageSize_ + i < DataSource_.Count)
                        {
                            listPageData.Add(DataSource_[(CurrentIndex_ - 1) * PageSize_ + i]);
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                return listPageData;
            }
            catch
            {
                return listPageData;
            }
        }

    }
    public enum JumpOperation
    {
        GoHome = 0,
        GoPrePrevious = 1,
        GoNext = 2,
        GoEnd = 3
    }
}
